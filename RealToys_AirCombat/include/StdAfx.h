/* PRE-COMPILED HEADER								*
 * Todos os includes que sejam raramente (ou nunca)	*
 * alterados, devem ser aqui colocados!				*/
#ifndef _StdAfx_h_
#define _StdAfx_h_

#include <stack>

/* Ogre headers */
#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreException.h>
#include <OgreNewt.h>
#include <OgreOggSound.h>
//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

/* raknet headers */
#include "RakMemoryOverride.h"
#include "RakAssert.h"
#include "StringCompressor.h"
#include "BitStream.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include "PluginInterface2.h"
#include "DS_Map.h"
#include "PacketPriority.h"
#include "RakNetTypes.h"
#include "RakString.h"
#include "ReplicaManager3.h"
#include "DS_Queue.h"
#include "DS_Hash.h"
#include "GetTime.h"


/* "in-this-project" headers */
#include "RPC3.h"

#endif
