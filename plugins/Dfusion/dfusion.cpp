#include <dfhack/Core.h>
#include <dfhack/Console.h>
#include <dfhack/Export.h>
#include <dfhack/PluginManager.h>
#include <dfhack/Process.h>
#include <vector>
#include <string>

#include "luamain.h"
#include "lua_Console.h"
#include "functioncall.h"

using std::vector;
using std::string;
using namespace DFHack;

static SDL::Mutex* mymutex=0;

DFhackCExport command_result dfusion (Core * c, vector <string> & parameters);
DFhackCExport command_result lua_run (Core * c, vector <string> & parameters);

 typedef
 int  (__thiscall *dfprint)(const char*, char, char,void *) ; 

DFhackCExport const char * plugin_name ( void )
{
    return "dfusion";
}

DFhackCExport command_result plugin_init ( Core * c, std::vector <PluginCommand> &commands)
{
    commands.clear();
	//maybe remake it to run automaticaly
	lua::RegisterConsole(lua::glua::Get(),&c->con);

    commands.push_back(PluginCommand("dfusion","Init dfusion system.",dfusion));
	commands.push_back(PluginCommand("lua", "Run interactive interpreter.\
\n              Options: <filename> = run <filename> instead",lua_run));
	
	mymutex=SDL_CreateMutex();
    return CR_OK;
}

DFhackCExport command_result plugin_shutdown ( Core * c )
{
	
// shutdown stuff
	return CR_OK;
}

DFhackCExport command_result plugin_onupdate ( Core * c )
{
    /*if(timering == true) //TODO maybe reuse this to make it run less often.
    {
        uint64_t time2 = GetTimeMs64();
        uint64_t delta = time2-timeLast;
        timeLast = time2;
        c->con.print("Time delta = %d ms\n", delta);
    }
    return CR_OK;*/
	SDL_mutexP(mymutex); 
	lua::state s=lua::glua::Get();
	s.getglobal("OnTick");
	if(s.is<lua::function>())
	{
		try{
			s.pcall();
		}
		catch(lua::exception &e)
		{
			c->con.printerr("Error OnTick:%s\n",e.what());
			c->con.msleep(1000);
		}
	}
	s.settop(0);
	SDL_mutexV(mymutex);
	return CR_OK;
}


DFhackCExport command_result lua_run (Core * c, vector <string> & parameters)
{
	//testing part{
	c->Suspend();
	FunctionCaller caller(c->p->getBase()); 
	std::vector <int> args;
	args.push_back((size_t)"Hello world");
	args.push_back(4);
	args.push_back(4);
	args.push_back(0);
	dfprint  mprint=(dfprint)(0x27F030+c->p->getBase());
	mprint("Hello world",4,4,0);
	caller.CallFunction((0x27F030),FunctionCaller::THIS_CALL,args);
	c->Resume();
	return CR_OK;
	//}end testing
	Console &con=c->con;
	SDL_mutexP(mymutex);
	lua::state s=lua::glua::Get();
	if(parameters.size()>0)
	{
		try{
			s.loadfile(parameters[0]); //load file
			s.pcall(0,0);// run it
		}
		catch(lua::exception &e)
		{
			con.printerr("Error:%s\n",e.what());
		}
	}
	else
	{
		//TODO interpreter...
	}
	s.settop(0);// clean up
	SDL_mutexV(mymutex);
	return CR_OK;
}
DFhackCExport command_result dfusion (Core * c, vector <string> & parameters)
{

	Console &con=c->con;
	con.print("%x\n",c->p->getBase());
	SDL_mutexP(mymutex);
	lua::state s=lua::glua::Get();
	
	try{
		s.loadfile("dfusion/init.lua"); //load script
		s.pcall(0,0);// run it
	}
	catch(lua::exception &e)
	{
		con.printerr("Error:%s\n",e.what());
	}
	s.settop(0);// clean up
	SDL_mutexV(mymutex);
	return CR_OK;
}
