/**
 * 
 * Interface analysis
 * 
 */ 
#pragma once

class IAnalyzer
{
private:
	const char* interface;
public:
	IAnalyzer(const char*);
	virtual ~IAnalyzer();

	static bool AnalyzeInterface(const char* name, void* vtable);

	virtual void Analyze(void* vtable) = 0;
};

class CVScriptAnalyzer : public IAnalyzer
{
public:
	CVScriptAnalyzer();
	virtual void Analyze(void* vt);
};

