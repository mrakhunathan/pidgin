typedef struct group *Gaim__Group;

#define group perl_group

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
#include <glib.h>

#undef group

#include "../perl-common.h"

#include "account.h"
#include "connection.h"
#include "debug.h"
#include "server.h"

typedef GaimAccount *     Gaim__Account;
typedef GaimConnection *  Gaim__Connection;
typedef GaimConversation *Gaim__Conversation;
typedef GaimPlugin *      Gaim__Plugin;
typedef struct buddy *    Gaim__BuddyList__Buddy;
typedef struct chat *     Gaim__BuddyList__Chat;
typedef struct group *    Gaim__BuddyList__Group;

typedef GaimDebugLevel Gaim__DebugLevel;
