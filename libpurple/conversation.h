/**
 * @file conversation.h Conversation base class API
 * @ingroup core
 */

/* purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */
#ifndef _PURPLE_CONVERSATION_H_
#define _PURPLE_CONVERSATION_H_

#define PURPLE_TYPE_CONVERSATION             (purple_conversation_get_type())
#define PURPLE_CONVERSATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), PURPLE_TYPE_CONVERSATION, PurpleConversation))
#define PURPLE_CONVERSATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), PURPLE_TYPE_CONVERSATION, PurpleConversationClass))
#define PURPLE_IS_CONVERSATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), PURPLE_TYPE_CONVERSATION))
#define PURPLE_IS_CONVERSATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), PURPLE_TYPE_CONVERSATION))
#define PURPLE_CONVERSATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), PURPLE_TYPE_CONVERSATION, PurpleConversationClass))

#define PURPLE_TYPE_CONVERSATION_MESSAGE     (purple_conversation_message_get_type())

/**************************************************************************/
/** Data Structures                                                       */
/**************************************************************************/

/** @copydoc _PurpleConversation */
typedef struct _PurpleConversation           PurpleConversation;
/** @copydoc _PurpleConversation */
typedef struct _PurpleConversationClass      PurpleConversationClass;

/** @copydoc _PurpleConversationUiOps */
typedef struct _PurpleConversationUiOps      PurpleConversationUiOps;
/** @copydoc _PurpleConversationMessage */
typedef struct _PurpleConversationMessage    PurpleConversationMessage;

/**
 * Conversation update type.
 */
typedef enum
{
	PURPLE_CONVERSATION_UPDATE_ADD = 0, /**< The buddy associated with the conversation
	                                         was added.   */
	PURPLE_CONVERSATION_UPDATE_REMOVE,  /**< The buddy associated with the conversation
	                                         was removed. */
	PURPLE_CONVERSATION_UPDATE_ACCOUNT, /**< The purple_account was changed. */
	PURPLE_CONVERSATION_UPDATE_TYPING,  /**< The typing state was updated. */
	PURPLE_CONVERSATION_UPDATE_UNSEEN,  /**< The unseen state was updated. */
	PURPLE_CONVERSATION_UPDATE_LOGGING, /**< Logging for this conversation was
	                                         enabled or disabled. */
	PURPLE_CONVERSATION_UPDATE_TOPIC,   /**< The topic for a chat was updated. */
	PURPLE_CONVERSATION_UPDATE_E2EE,    /**< The End-to-end encryption state was
	                                         updated. */
	/*
	 * XXX These need to go when we implement a more generic core/UI event
	 * system.
	 */
	PURPLE_CONVERSATION_ACCOUNT_ONLINE,  /**< One of the user's accounts went online.  */
	PURPLE_CONVERSATION_ACCOUNT_OFFLINE, /**< One of the user's accounts went offline. */
	PURPLE_CONVERSATION_UPDATE_AWAY,     /**< The other user went away.                */
	PURPLE_CONVERSATION_UPDATE_ICON,     /**< The other user's buddy icon changed.     */
	PURPLE_CONVERSATION_UPDATE_NAME,
	PURPLE_CONVERSATION_UPDATE_TITLE,
	PURPLE_CONVERSATION_UPDATE_CHATLEFT,

	PURPLE_CONVERSATION_UPDATE_FEATURES  /**< The features for a chat have changed */

} PurpleConversationUpdateType;

/**
 * Flags applicable to a message. Most will have send, recv or system.
 */
typedef enum /*< flags >*/
{
	PURPLE_MESSAGE_SEND        = 0x0001, /**< Outgoing message.        */
	PURPLE_MESSAGE_RECV        = 0x0002, /**< Incoming message.        */
	PURPLE_MESSAGE_SYSTEM      = 0x0004, /**< System message.          */
	PURPLE_MESSAGE_AUTO_RESP   = 0x0008, /**< Auto response.           */
	PURPLE_MESSAGE_ACTIVE_ONLY = 0x0010,  /**< Hint to the UI that this
	                                           message should not be
	                                           shown in conversations
	                                           which are only open for
	                                           internal UI purposes
	                                           (e.g. for contact-aware
	                                           conversations).         */
	PURPLE_MESSAGE_NICK        = 0x0020, /**< Contains your nick.      */
	PURPLE_MESSAGE_NO_LOG      = 0x0040, /**< Do not log.              */
	PURPLE_MESSAGE_WHISPER     = 0x0080, /**< Whispered message.       */
	PURPLE_MESSAGE_ERROR       = 0x0200, /**< Error message.           */
	PURPLE_MESSAGE_DELAYED     = 0x0400, /**< Delayed message.         */
	PURPLE_MESSAGE_RAW         = 0x0800, /**< "Raw" message - don't
	                                           apply formatting        */
	PURPLE_MESSAGE_IMAGES      = 0x1000, /**< Message contains images  */
	PURPLE_MESSAGE_NOTIFY      = 0x2000, /**< Message is a notification */
	PURPLE_MESSAGE_NO_LINKIFY  = 0x4000, /**< Message should not be auto-
										      linkified */
	PURPLE_MESSAGE_INVISIBLE   = 0x8000  /**< Message should not be displayed */
} PurpleMessageFlags;

#include <glib.h>
#include <glib-object.h>

/**************************************************************************/
/** PurpleConversation                                                    */
/**************************************************************************/
/**
 * A core representation of a conversation between two or more people.
 *
 * The conversation can be an IM or a chat.
 *
 * Note: When a conversation is destroyed with the last g_object_unref(), the
 *       specified conversation is removed from the parent window. If this
 *       conversation is the only one contained in the parent window, that
 *       window is also destroyed.
 */
struct _PurpleConversation
{
	GObject gparent;

	/** The UI data associated with this conversation. This is a convenience
	 *  field provided to the UIs -- it is not used by the libpurple core.
	 */
	gpointer ui_data;
};

/** Base class for all #PurpleConversation's */
struct _PurpleConversationClass {
	GObjectClass parent_class;

	/** Writes a message to a chat or IM conversation.
	 *  @see purple_conversation_write_message()
	 */
	void (*write_message)(PurpleConversation *conv, const char *who,
			const char *message, PurpleMessageFlags flags, time_t mtime);

	/*< private >*/
	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

#include "account.h"
#include "buddyicon.h"
#include "e2ee.h"
#include "log.h"

/**************************************************************************/
/** PurpleConversationUiOps                                               */
/**************************************************************************/
/**
 * Conversation operations and events.
 *
 * Any UI representing a conversation must assign a filled-out
 * PurpleConversationUiOps structure to the PurpleConversation.
 */
struct _PurpleConversationUiOps
{
	/** Called when @a conv is created (but before the @ref
	 *  conversation-created signal is emitted).
	 */
	void (*create_conversation)(PurpleConversation *conv);

	/** Called just before @a conv is freed. */
	void (*destroy_conversation)(PurpleConversation *conv);
	/** Write a message to a chat.  If this field is %NULL, libpurple will
	 *  fall back to using #write_conv.
	 *  @see purple_chat_conversation_write()
	 */
	void (*write_chat)(PurpleChatConversation *chat, const char *who,
	                  const char *message, PurpleMessageFlags flags,
	                  time_t mtime);
	/** Write a message to an IM conversation.  If this field is %NULL,
	 *  libpurple will fall back to using #write_conv.
	 *  @see purple_im_conversation_write()
	 */
	void (*write_im)(PurpleIMConversation *im, const char *who,
	                 const char *message, PurpleMessageFlags flags,
	                 time_t mtime);
	/** Write a message to a conversation.  This is used rather than the
	 *  chat- or im-specific ops for errors, system messages (such as "x is
	 *  now know as y"), and as the fallback if #write_im and #write_chat
	 *  are not implemented.  It should be implemented, or the UI will miss
	 *  conversation error messages and your users will hate you.
	 *
	 *  @see purple_conversation_write()
	 */
	void (*write_conv)(PurpleConversation *conv,
	                   const char *name,
	                   const char *alias,
	                   const char *message,
	                   PurpleMessageFlags flags,
	                   time_t mtime);

	/** Add @a cbuddies to a chat.
	 *  @cbuddies:      A @c GList of #PurpleChatUser structs.
	 *  @new_arrivals:  Whether join notices should be shown.
	 *                       (Join notices are actually written to the
	 *                       conversation by
	 *                       #purple_chat_conversation_add_users().)
	 */
	void (*chat_add_users)(PurpleChatConversation *chat,
	                       GList *cbuddies,
	                       gboolean new_arrivals);
	/** Rename the user in this chat named @a old_name to @a new_name.  (The
	 *  rename message is written to the conversation by libpurple.)
	 *  @new_alias:  @a new_name's new alias, if they have one.
	 *  @see purple_chat_conversation_add_users()
	 */
	void (*chat_rename_user)(PurpleChatConversation *chat, const char *old_name,
	                         const char *new_name, const char *new_alias);
	/** Remove @a users from a chat.
	 *  @users:    A @c GList of <tt>const char *</tt>s.
	 *  @see purple_chat_conversation_rename_user()
	 */
	void (*chat_remove_users)(PurpleChatConversation *chat, GList *users);
	/** Called when a user's flags are changed.
	 *  @see purple_chat_user_set_flags()
	 */
	void (*chat_update_user)(PurpleChatUser *cb);

	/** Present this conversation to the user; for example, by displaying
	 *  the IM dialog.
	 */
	void (*present)(PurpleConversation *conv);

	/** If this UI has a concept of focus (as in a windowing system) and
	 *  this conversation has the focus, return %TRUE; otherwise, return
	 *  %FALSE.
	 */
	gboolean (*has_focus)(PurpleConversation *conv);

	/* Custom Smileys */
	gboolean (*custom_smiley_add)(PurpleConversation *conv, const char *smile,
	                            gboolean remote);
	void (*custom_smiley_write)(PurpleConversation *conv, const char *smile,
	                            const guchar *data, gsize size);
	void (*custom_smiley_close)(PurpleConversation *conv, const char *smile);

	/** Prompt the user for confirmation to send @a message.  This function
	 *  should arrange for the message to be sent if the user accepts.  If
	 *  this field is %NULL, libpurple will fall back to using
	 *  #purple_request_action().
	 */
	void (*send_confirm)(PurpleConversation *conv, const char *message);

	/*< private >*/
	void (*_purple_reserved1)(void);
	void (*_purple_reserved2)(void);
	void (*_purple_reserved3)(void);
	void (*_purple_reserved4)(void);
};

G_BEGIN_DECLS

/**************************************************************************/
/** @name Conversation API                                                */
/**************************************************************************/
/*@{*/

/**
 * Returns the GType for the Conversation object.
 */
GType purple_conversation_get_type(void);

/**
 * Present a conversation to the user. This allows core code to initiate a
 * conversation by displaying the IM dialog.
 * @conv: The conversation to present
 */
void purple_conversation_present(PurpleConversation *conv);

/**
 * Sets the specified conversation's UI operations structure.
 *
 * @conv: The conversation.
 * @ops:  The UI conversation operations structure.
 */
void purple_conversation_set_ui_ops(PurpleConversation *conv,
								  PurpleConversationUiOps *ops);

/**
 * Returns the specified conversation's UI operations structure.
 *
 * @conv: The conversation.
 *
 * Returns: The operations structure.
 */
PurpleConversationUiOps *purple_conversation_get_ui_ops(const PurpleConversation *conv);

/**
 * Sets the specified conversation's purple_account.
 *
 * This purple_account represents the user using purple, not the person the user
 * is having a conversation/chat/flame with.
 *
 * @conv: The conversation.
 * @account: The purple_account.
 */
void purple_conversation_set_account(PurpleConversation *conv,
                                   PurpleAccount *account);

/**
 * Returns the specified conversation's purple_account.
 *
 * This purple_account represents the user using purple, not the person the user
 * is having a conversation/chat/flame with.
 *
 * @conv: The conversation.
 *
 * Returns: The conversation's purple_account.
 */
PurpleAccount *purple_conversation_get_account(const PurpleConversation *conv);

/**
 * Returns the specified conversation's purple_connection.
 *
 * @conv: The conversation.
 *
 * Returns: The conversation's purple_connection.
 */
PurpleConnection *purple_conversation_get_connection(const PurpleConversation *conv);

/**
 * Sets the specified conversation's title.
 *
 * @conv:  The conversation.
 * @title: The title.
 */
void purple_conversation_set_title(PurpleConversation *conv, const char *title);

/**
 * Returns the specified conversation's title.
 *
 * @conv: The conversation.
 *
 * Returns: The title.
 */
const char *purple_conversation_get_title(const PurpleConversation *conv);

/**
 * Automatically sets the specified conversation's title.
 *
 * This function takes OPT_IM_ALIAS_TAB into account, as well as the
 * user's alias.
 *
 * @conv: The conversation.
 */
void purple_conversation_autoset_title(PurpleConversation *conv);

/**
 * Sets the specified conversation's name.
 *
 * @conv: The conversation.
 * @name: The conversation's name.
 */
void purple_conversation_set_name(PurpleConversation *conv, const char *name);

/**
 * Returns the specified conversation's name.
 *
 * @conv: The conversation.
 *
 * Returns: The conversation's name. If the conversation is an IM with a PurpleBuddy,
 *         then it's the name of the PurpleBuddy.
 */
const char *purple_conversation_get_name(const PurpleConversation *conv);

/**
 * Sets current E2EE state for the conversation.
 *
 * @conv:  The conversation.
 * @state: The E2EE state.
 */
void
purple_conversation_set_e2ee_state(PurpleConversation *conv,
	PurpleE2eeState *state);

/**
 * Gets current conversation's E2EE state.
 *
 * @conv: The conversation.
 *
 * Returns: Current E2EE state for conversation.
 */
PurpleE2eeState *
purple_conversation_get_e2ee_state(PurpleConversation *conv);

/**
 * Enables or disables logging for this conversation.
 *
 * @conv: The conversation.
 * @log:  %TRUE if logging should be enabled, or %FALSE otherwise.
 */
void purple_conversation_set_logging(PurpleConversation *conv, gboolean log);

/**
 * Returns whether or not logging is enabled for this conversation.
 *
 * @conv: The conversation.
 *
 * Returns: %TRUE if logging is enabled, or %FALSE otherwise.
 */
gboolean purple_conversation_is_logging(const PurpleConversation *conv);

/**
 * Closes any open logs for this conversation.
 *
 * Note that new logs will be opened as necessary (e.g. upon receipt of a
 * message, if the conversation has logging enabled. To disable logging for
 * the remainder of the conversation, use purple_conversation_set_logging().
 *
 * @conv: The conversation.
 */
void purple_conversation_close_logs(PurpleConversation *conv);

/**
 * Writes to a conversation window.
 *
 * This function should not be used to write IM or chat messages. Use
 * purple_conversation_write_message() instead. This function will
 * most likely call this anyway, but it may do it's own formatting,
 * sound playback, etc. depending on whether the conversation is a chat or an
 * IM.
 *
 * This can be used to write generic messages, such as "so and so closed
 * the conversation window."
 *
 * @conv:    The conversation.
 * @who:     The user who sent the message.
 * @message: The message.
 * @flags:   The message flags.
 * @mtime:   The time the message was sent.
 *
 * @see purple_conversation_write_message()
 */
void purple_conversation_write(PurpleConversation *conv, const char *who,
		const char *message, PurpleMessageFlags flags,
		time_t mtime);

/**
 * Writes to a chat or an IM.
 *
 * @conv:    The conversation.
 * @who:     The user who sent the message.
 * @message: The message to write.
 * @flags:   The message flags.
 * @mtime:   The time the message was sent.
 */
void purple_conversation_write_message(PurpleConversation *conv,
		const char *who, const char *message,
		PurpleMessageFlags flags, time_t mtime);

/**
 * Sends a message to this conversation. This function calls
 * purple_conversation_send_with_flags() with no additional flags.
 *
 * @conv:    The conversation.
 * @message: The message to send.
 */
void purple_conversation_send(PurpleConversation *conv, const char *message);

/**
 * Sends a message to this conversation with specified flags.
 *
 * @conv:    The conversation.
 * @message: The message to send.
 * @flags:   The PurpleMessageFlags flags to use in addition to
 *                PURPLE_MESSAGE_SEND.
 */
void purple_conversation_send_with_flags(PurpleConversation *conv, const char *message,
		PurpleMessageFlags flags);

/**
	Set the features as supported for the given conversation.
	@conv:      The conversation
	@features:  Bitset defining supported features
*/
void purple_conversation_set_features(PurpleConversation *conv,
		PurpleConnectionFlags features);


/**
	Get the features supported by the given conversation.
	@conv:  The conversation
*/
PurpleConnectionFlags purple_conversation_get_features(PurpleConversation *conv);

/**
 * Determines if a conversation has focus
 *
 * @conv:    The conversation.
 *
 * Returns: %TRUE if the conversation has focus, %FALSE if
 * it does not or the UI does not have a concept of conversation focus
 */
gboolean purple_conversation_has_focus(PurpleConversation *conv);

/**
 * Updates the visual status and UI of a conversation.
 *
 * @conv: The conversation.
 * @type: The update type.
 */
void purple_conversation_update(PurpleConversation *conv, PurpleConversationUpdateType type);

/**
 * Retrieve the message history of a conversation.
 *
 * @conv:   The conversation
 *
 * Returns:  A GList of PurpleConversationMessage's. The must not modify the list or the data within.
 *          The list contains the newest message at the beginning, and the oldest message at
 *          the end.
 */
GList *purple_conversation_get_message_history(PurpleConversation *conv);

/**
 * Clear the message history of a conversation.
 *
 * @conv:  The conversation
 */
void purple_conversation_clear_message_history(PurpleConversation *conv);

/**
 * Set the UI data associated with this conversation.
 *
 * @conv:			The conversation.
 * @ui_data:		A pointer to associate with this conversation.
 */
void purple_conversation_set_ui_data(PurpleConversation *conv, gpointer ui_data);

/**
 * Get the UI data associated with this conversation.
 *
 * @conv:			The conversation.
 *
 * Returns: The UI data associated with this conversation.  This is a
 *         convenience field provided to the UIs--it is not
 *         used by the libpurple core.
 */
gpointer purple_conversation_get_ui_data(const PurpleConversation *conv);

/**
 * Sends a message to a conversation after confirming with
 * the user.
 *
 * This function is intended for use in cases where the user
 * hasn't explicitly and knowingly caused a message to be sent.
 * The confirmation ensures that the user isn't sending a
 * message by mistake.
 *
 * @conv:    The conversation.
 * @message: The message to send.
 */
void purple_conversation_send_confirm(PurpleConversation *conv, const char *message);

/**
 * Adds a smiley to the conversation's smiley tree. If this returns
 * %TRUE you should call purple_conversation_custom_smiley_write() one or more
 * times, and then purple_conversation_custom_smiley_close(). If this returns
 * %FALSE, either the conv or smile were invalid, or the icon was
 * found in the cache. In either case, calling write or close would
 * be an error.
 *
 * @conv: The conversation to associate the smiley with.
 * @smile: The text associated with the smiley
 * @cksum_type: The type of checksum.
 * @chksum: The checksum, as a NUL terminated base64 string.
 * @remote: %TRUE if the custom smiley is set by the remote user (buddy).
 * Returns:      %TRUE if an icon is expected, else FALSE. Note that
 *              it is an error to never call purple_conversation_custom_smiley_close if
 *              this function returns %TRUE, but an error to call it if
 *              %FALSE is returned.
 */

gboolean purple_conversation_custom_smiley_add(PurpleConversation *conv, const char *smile,
                                      const char *cksum_type, const char *chksum,
									  gboolean remote);

/**
 * Updates the image associated with the current smiley.
 *
 * @conv: The conversation associated with the smiley.
 * @smile: The text associated with the smiley.
 * @data: The actual image data.
 * @size: The length of the data.
 */

void purple_conversation_custom_smiley_write(PurpleConversation *conv,
                                   const char *smile,
                                   const guchar *data,
                                   gsize size);

/**
 * Close the custom smiley, all data has been written with
 * purple_conversation_custom_smiley_write, and it is no longer valid
 * to call that function on that smiley.
 *
 * @conv: The purple conversation associated with the smiley.
 * @smile: The text associated with the smiley
 */

void purple_conversation_custom_smiley_close(PurpleConversation *conv, const char *smile);

/**
 * Retrieves the extended menu items for the conversation.
 *
 * @conv: The conversation.
 *
 * Returns:  A list of PurpleMenuAction items, harvested by the
 *          chat-extended-menu signal. The list and the menuaction
 *          items should be freed by the caller.
 */
GList * purple_conversation_get_extended_menu(PurpleConversation *conv);

/**
 * Perform a command in a conversation. Similar to @see purple_cmd_do_command
 *
 * @conv:    The conversation.
 * @cmdline: The entire command including the arguments.
 * @markup:  %NULL, or the formatted command line.
 * @error:   If the command failed errormsg is filled in with the appropriate error
 *                message, if not %NULL. It must be freed by the caller with g_free().
 *
 * Returns:  %TRUE if the command was executed successfully, %FALSE otherwise.
 */
gboolean purple_conversation_do_command(PurpleConversation *conv,
		const gchar *cmdline, const gchar *markup, gchar **error);

/**
 * Gets the maximum message size in bytes for the conversation.
 *
 * @see PurplePluginProtocolInfo#get_max_message_size
 *
 * @conv: The conversation to query.
 *
 * Returns: Maximum message size, 0 if unspecified, -1 for infinite.
 */
gssize
purple_conversation_get_max_message_size(PurpleConversation *conv);

/*@}*/

/**************************************************************************/
/** @name Conversation Helper API                                         */
/**************************************************************************/
/*@{*/

/**
 * Presents an IM-error to the user
 *
 * This is a helper function to find a conversation, write an error to it, and
 * raise the window.  If a conversation with this user doesn't already exist,
 * the function will return FALSE and the calling function can attempt to present
 * the error another way (purple_notify_error, most likely)
 *
 * @who:     The user this error is about
 * @account: The account this error is on
 * @what:    The error
 * Returns:        TRUE if the error was presented, else FALSE
 */
gboolean purple_conversation_present_error(const char *who, PurpleAccount *account, const char *what);

/*@}*/

/**************************************************************************/
/** @name Conversation Message API                                        */
/**************************************************************************/
/*@{*/

/**
 * Returns the GType for the PurpleConversationMessage boxed structure.
 */
GType purple_conversation_message_get_type(void);

/**
 * Get the sender from a PurpleConversationMessage
 *
 * @msg:   A PurpleConversationMessage
 *
 * Returns:   The name of the sender of the message
 */
const char *purple_conversation_message_get_sender(const PurpleConversationMessage *msg);

/**
 * Get the message from a PurpleConversationMessage
 *
 * @msg:   A PurpleConversationMessage
 *
 * Returns:   The name of the sender of the message
 */
const char *purple_conversation_message_get_message(const PurpleConversationMessage *msg);

/**
 * Get the message-flags of a PurpleConversationMessage
 *
 * @msg:   A PurpleConversationMessage
 *
 * Returns:   The message flags
 */
PurpleMessageFlags purple_conversation_message_get_flags(const PurpleConversationMessage *msg);

/**
 * Get the timestamp of a PurpleConversationMessage
 *
 * @msg:   A PurpleConversationMessage
 *
 * Returns:   The timestamp of the message
 */
time_t purple_conversation_message_get_timestamp(const PurpleConversationMessage *msg);

/**
 * Get the alias from a PurpleConversationMessage
 *
 * @msg:   A PurpleConversationMessage
 *
 * Returns:   The alias of the sender of the message
 */
const char *purple_conversation_message_get_alias(const PurpleConversationMessage *msg);

/**
 * Get the conversation associated with the PurpleConversationMessage
 *
 * @msg:   A PurpleConversationMessage
 *
 * Returns:   The conversation
 */
PurpleConversation *purple_conversation_message_get_conversation(const PurpleConversationMessage *msg);

/*@}*/

G_END_DECLS

#endif /* _PURPLE_CONVERSATION_H_ */
