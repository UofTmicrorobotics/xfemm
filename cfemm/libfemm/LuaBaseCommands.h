/* Copyright 2016 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#ifndef LUABASECOMMANDS_H
#define LUABASECOMMANDS_H

class lua_State;

namespace femm
{

class LuaInstance;

/**
 * LuaBaseCommands registers the common lua commands usually provided by the FEMM UI.
 * The commmon Lua command set is described in section 3.2 of the FEMM manual.
 *
 * Since most commands in this set are UI-related, this class can only provide a dummy interface.
 * To actually implement these GUI-related commands, the easiest way is to inherit from it and register the commands in the constructor.
 *
 * Note: mathlink commands (mlput, mlopen, mlclose) are not provided.
 */
class LuaBaseCommands
{
public:
    /**
     * @brief Register the common command set with the given LuaInstance
     * @param li a LuaInstance
     */
    static void registerCommands(LuaInstance &li );

protected:
    static int luaError(lua_State *L);
    static int luaExit(lua_State *L);
    static int luaHidePointProps(lua_State *L);
    static int luaMessageBox(lua_State *L);
    static int luaNewDocument(lua_State *L);
    static int luaOpenDocument(lua_State *L);
    static int luaPause(lua_State *L);
    static int luaPromptBox(lua_State *L);
    static int luaShowConsole(lua_State *);
    static int luaShowPointProps(lua_State *L);
    static int luaSetWorkingDirectory(lua_State *L);
    static int luaMakePlot(lua_State *L);
};

} /* namespace FemmLua*/

#endif /* LUABASECOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4: