/**
 * VTable analysis and other things
 * 
 */ 
#include "analyze.h"
#include "common.h"

#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static std::vector<IAnalyzer*> g_analyzers;

IAnalyzer::IAnalyzer(const char* interface)
{
	this->interface = interface;
	g_analyzers.push_back(this);
}

IAnalyzer::~IAnalyzer()
{

}

bool IAnalyzer::AnalyzeInterface(const char* interface, void* vt)
{
	for(auto x : g_analyzers)
	{
		if(strcmp(x->interface, interface) == 0)
		{
			x->Analyze(vt);
			return true;
		}
	}
	return false;
}

/**
 * 
 * VScript Interface Analyzer
 * 
 */ 

CVScriptAnalyzer::CVScriptAnalyzer() :
	IAnalyzer("VScriptManager010")
{

}

void CVScriptAnalyzer::Analyze(void* vt)
{

}


/* Static initialization of initializers */
static CVScriptAnalyzer g_vscript_anal;