/**
 * @file connection.c Connection API
 * @ingroup core
 *
 * gaim
 *
 * Gaim is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "internal.h"
#include "account.h"
#include "blist.h"
#include "connection.h"
#include "debug.h"
#include "log.h"
#include "notify.h"
#include "prefs.h"
#include "request.h"
#include "server.h"
#include "signals.h"
#include "util.h"

static GList *connections = NULL;
static GList *connections_connecting = NULL;
static GaimConnectionUiOps *connection_ui_ops = NULL;

static int connections_handle;

void
gaim_connection_new(GaimAccount *account, gboolean regist, const char *password)
{
	GaimConnection *gc;
	GaimPlugin *prpl;
	GaimPluginProtocolInfo *prpl_info;

	g_return_if_fail(account != NULL);

	prpl = gaim_find_prpl(gaim_account_get_protocol_id(account));

	if (prpl != NULL)
		prpl_info = GAIM_PLUGIN_PROTOCOL_INFO(prpl);
	else {
		gchar *message;

		message = g_strdup_printf(_("Missing protocol plugin for %s"),
			gaim_account_get_username(account));
		gaim_notify_error(NULL, regist ? _("Registration Error") :
						  _("Connection Error"), message, NULL);
		g_free(message);
		return;
	}

	if (regist)
	{
		if (prpl_info->register_user == NULL)
			return;
	}
	else
	{
		if ((password == NULL) &&
			!(prpl_info->options & OPT_PROTO_NO_PASSWORD) &&
			!(prpl_info->options & OPT_PROTO_PASSWORD_OPTIONAL))
		{
			gaim_debug_error("connection", "Can not connect to account %s without "
							 "a password.\n", gaim_account_get_username(account));
			return;
		}
	}

	gc = g_new0(GaimConnection, 1);
	gc->prpl = prpl;
	gc->password = g_strdup(password);
	gaim_connection_set_account(gc, account);
	gaim_connection_set_state(gc, GAIM_CONNECTING);
	connections = g_list_append(connections, gc);
	gaim_account_set_connection(account, gc);

	gaim_signal_emit(gaim_connections_get_handle(), "signing-on", gc);

	if (regist)
	{
		gaim_debug_info("connection", "Registering.  gc = %p\n", gc);

		/* set this so we don't auto-reconnect after registering */
		gc->wants_to_die = TRUE;

		prpl_info->register_user(account);
	}
	else
	{
		gaim_debug_info("connection", "Connecting. gc = %p\n", gc);

		gaim_signal_emit(gaim_accounts_get_handle(), "account-connecting", account);
		prpl_info->login(account, gaim_account_get_active_status(account));
	}
}

void
gaim_connection_destroy(GaimConnection *gc)
{
	GaimAccount *account;

	g_return_if_fail(gc != NULL);

	account = gaim_connection_get_account(gc);

	if (gaim_connection_get_state(gc) != GAIM_DISCONNECTED) {
		GList *wins;
		GaimPresence *presence = NULL;

		gaim_debug_info("connection", "Disconnecting connection %p\n", gc);

		if (gaim_connection_get_state(gc) != GAIM_CONNECTING)
			gaim_blist_remove_account(gaim_connection_get_account(gc));

		gaim_signal_emit(gaim_connections_get_handle(), "signing-off", gc);

		serv_close(gc);

		connections = g_list_remove(connections, gc);

		gaim_connection_set_state(gc, GAIM_DISCONNECTED);

		/* LOG	system_log(log_signoff, gc, NULL,
		   OPT_LOG_BUDDY_SIGNON | OPT_LOG_MY_SIGNON); */
		gaim_signal_emit(gaim_connections_get_handle(), "signed-off", gc);

		presence = gaim_account_get_presence(account);
		if (gaim_presence_is_online(presence) == TRUE)
			gaim_presence_set_status_active(presence, "offline", TRUE);

		/*
		 * XXX This is a hack! Remove this and replace it with a better event
		 *     notification system.
		 */
		for (wins = gaim_get_windows(); wins != NULL; wins = wins->next) {
			GaimConvWindow *win = (GaimConvWindow *)wins->data;
			gaim_conversation_update(gaim_conv_window_get_conversation_at(win, 0),
									 GAIM_CONV_ACCOUNT_OFFLINE);
		}

		gaim_request_close_with_handle(gc);
		gaim_notify_close_with_handle(gc);

		return;
	}

	gaim_debug_info("connection", "Destroying connection %p\n", gc);

	gaim_account_set_connection(account, NULL);

	if (gc->password != NULL)
		g_free(gc->password);

	if (gc->display_name != NULL)
		g_free(gc->display_name);

	if (gc->disconnect_timeout)
		gaim_timeout_remove(gc->disconnect_timeout);

	g_free(gc);
}

/*
 * d:)->-<
 *
 * d:O-\-<
 *
 * d:D-/-<
 *
 * d8D->-< DANCE!
 */

void
gaim_connection_set_state(GaimConnection *gc, GaimConnectionState state)
{
	GaimConnectionUiOps *ops;

	g_return_if_fail(gc != NULL);

	if (gc->state == state)
		return;

	gc->state = state;

	ops = gaim_connections_get_ui_ops();

	if (gc->state == GAIM_CONNECTING) {
		connections_connecting = g_list_append(connections_connecting, gc);
	}
	else {
		connections_connecting = g_list_remove(connections_connecting, gc);
	}

	if (gc->state == GAIM_CONNECTED) {
		GaimBlistNode *gnode,*cnode,*bnode;
		GList *wins;
		GList *add_buds = NULL;
		GaimAccount *account;
		GaimPresence *presence;

		account = gaim_connection_get_account(gc);
		presence = gaim_account_get_presence(account);

		/* Set the time the account came online */
		time(&gc->login_time);

		if (gaim_prefs_get_bool("/core/logging/log_system") &&
		   gaim_prefs_get_bool("/core/logging/log_own_states")){
			GaimLog *log = gaim_account_get_log(account);
			char *msg = g_strdup_printf("+++ %s signed on",
										gaim_account_get_username(account));
			gaim_log_write(log, GAIM_MESSAGE_SYSTEM,
						   gaim_account_get_username(account), gc->login_time,
						   msg);
			g_free(msg);
		}

		if (ops != NULL && ops->connected != NULL)
			ops->connected(gc);

		gaim_blist_show();
		gaim_blist_add_account(account);

		/*
		 * XXX This is a hack! Remove this and replace it with a better event
		 *     notification system.
		 */
		for (wins = gaim_get_windows(); wins != NULL; wins = wins->next) {
			GaimConvWindow *win = (GaimConvWindow *)wins->data;
			gaim_conversation_update(gaim_conv_window_get_conversation_at(win, 0),
									 GAIM_CONV_ACCOUNT_ONLINE);
		}

		gaim_signal_emit(gaim_connections_get_handle(), "signed-on", gc);

		/* let the prpl know what buddies we pulled out of the local list */
		/* XXX - Remove this and let the prpl take care of it itself? */
		for (gnode = gaim_get_blist()->root; gnode; gnode = gnode->next) {
			if(!GAIM_BLIST_NODE_IS_GROUP(gnode))
				continue;
			for(cnode = gnode->child; cnode; cnode = cnode->next) {
				if(!GAIM_BLIST_NODE_IS_CONTACT(cnode))
					continue;
				for(bnode = cnode->child; bnode; bnode = bnode->next) {
					GaimBuddy *b;
					if(!GAIM_BLIST_NODE_IS_BUDDY(bnode))
						continue;

					b = (GaimBuddy *)bnode;
					if(b->account == gc->account) {
						add_buds = g_list_append(add_buds, b);
					}
				}
			}
		}

		if(add_buds) {
			serv_add_buddies(gc, add_buds);
			g_list_free(add_buds);
		}

		serv_set_permit_deny(gc);
	}
	else if (gc->state == GAIM_DISCONNECTED) {
		GaimAccount *account = gaim_connection_get_account(gc);

		if(gaim_prefs_get_bool("/core/logging/log_system") &&
		   gaim_prefs_get_bool("/core/logging/log_own_states")){
			GaimLog *log = gaim_account_get_log(account);
			char *msg = g_strdup_printf("+++ %s signed off",
										gaim_account_get_username(account));
			gaim_log_write(log, GAIM_MESSAGE_SYSTEM,
						   gaim_account_get_username(account), time(NULL),
						   msg);
			g_free(msg);
		}

		gaim_account_destroy_log(account);

		if (ops != NULL && ops->disconnected != NULL)
			ops->disconnected(gc);
	}
}

void
gaim_connection_set_account(GaimConnection *gc, GaimAccount *account)
{
	g_return_if_fail(gc != NULL);
	g_return_if_fail(account != NULL);

	gc->account = account;
}

void
gaim_connection_set_display_name(GaimConnection *gc, const char *name)
{
	g_return_if_fail(gc != NULL);

	if (gc->display_name != NULL)
		g_free(gc->display_name);

	gc->display_name = (name == NULL ? NULL : g_strdup(name));
}

GaimConnectionState
gaim_connection_get_state(const GaimConnection *gc)
{
	g_return_val_if_fail(gc != NULL, GAIM_DISCONNECTED);

	return gc->state;
}

GaimAccount *
gaim_connection_get_account(const GaimConnection *gc)
{
	g_return_val_if_fail(gc != NULL, NULL);

	return gc->account;
}

const char *
gaim_connection_get_password(const GaimConnection *gc)
{
	g_return_val_if_fail(gc != NULL, NULL);

	return gc->password;
}

const char *
gaim_connection_get_display_name(const GaimConnection *gc)
{
	g_return_val_if_fail(gc != NULL, NULL);

	return gc->display_name;
}

void
gaim_connection_update_progress(GaimConnection *gc, const char *text,
								size_t step, size_t count)
{
	GaimConnectionUiOps *ops;

	g_return_if_fail(gc   != NULL);
	g_return_if_fail(text != NULL);
	g_return_if_fail(step < count);
	g_return_if_fail(count > 1);

	ops = gaim_connections_get_ui_ops();

	if (ops != NULL && ops->connect_progress != NULL)
		ops->connect_progress(gc, text, step, count);
}

void
gaim_connection_notice(GaimConnection *gc, const char *text)
{
	GaimConnectionUiOps *ops;

	g_return_if_fail(gc   != NULL);
	g_return_if_fail(text != NULL);

	ops = gaim_connections_get_ui_ops();

	if (ops != NULL && ops->notice != NULL)
		ops->notice(gc, text);
}

gboolean
gaim_connection_disconnect_cb(gpointer data)
{
	GaimAccount *account = data;

	gaim_account_disconnect(account);

	return FALSE;
}

void
gaim_connection_error(GaimConnection *gc, const char *text)
{
	GaimConnectionUiOps *ops;

	g_return_if_fail(gc   != NULL);
	g_return_if_fail(text != NULL);

	/* If we've already got one error, we don't need any more */
	if (gc->disconnect_timeout)
		return;

	ops = gaim_connections_get_ui_ops();

	if (ops != NULL) {
		if (ops->report_disconnect != NULL)
			ops->report_disconnect(gc, text);
	}

	gc->disconnect_timeout = gaim_timeout_add(0, gaim_connection_disconnect_cb,
			gaim_connection_get_account(gc));
}

void
gaim_connections_disconnect_all(void)
{
	GList *l;
	GaimConnection *gc;

	while ((l = gaim_connections_get_all()) != NULL) {
		gc = l->data;
		gc->wants_to_die = TRUE;
		gaim_account_disconnect(gc->account);
	}
}

GList *
gaim_connections_get_all(void)
{
	return connections;
}

GList *
gaim_connections_get_connecting(void)
{
	return connections_connecting;
}

void
gaim_connections_set_ui_ops(GaimConnectionUiOps *ops)
{
	connection_ui_ops = ops;
}

GaimConnectionUiOps *
gaim_connections_get_ui_ops(void)
{
	return connection_ui_ops;
}

void
gaim_connections_init(void)
{
	void *handle = gaim_connections_get_handle();

	gaim_signal_register(handle, "signing-on",
						 gaim_marshal_VOID__POINTER, NULL, 1,
						 gaim_value_new(GAIM_TYPE_SUBTYPE,
										GAIM_SUBTYPE_CONNECTION));

	gaim_signal_register(handle, "signed-on",
						 gaim_marshal_VOID__POINTER, NULL, 1,
						 gaim_value_new(GAIM_TYPE_SUBTYPE,
										GAIM_SUBTYPE_CONNECTION));

	gaim_signal_register(handle, "signing-off",
						 gaim_marshal_VOID__POINTER, NULL, 1,
						 gaim_value_new(GAIM_TYPE_SUBTYPE,
										GAIM_SUBTYPE_CONNECTION));

	gaim_signal_register(handle, "signed-off",
						 gaim_marshal_VOID__POINTER, NULL, 1,
						 gaim_value_new(GAIM_TYPE_SUBTYPE,
										GAIM_SUBTYPE_CONNECTION));
}

void
gaim_connections_uninit(void)
{
	gaim_signals_unregister_by_instance(gaim_connections_get_handle());
}

void *
gaim_connections_get_handle(void)
{
	return &connections_handle;
}
