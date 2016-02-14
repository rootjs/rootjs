#include "NodeApplication.h"

#include "ObjectProxy.h"

#include <TROOT.h>
#include <TInterpreter.h>
#include <TSystem.h>
#include <TBenchmark.h>
#include <TStyle.h>
#include <TError.h>
#include <Getline.h>
#include <TEnv.h>

namespace rootJS
{
	NodeApplication::NodeApplication(const char* acn, Int_t* argc, char** argv):
		TApplication( acn, argc, argv )
	{
		// save current interpreter context
		gInterpreter->SaveContext();
		gInterpreter->SaveGlobalsContext();

		// prevent crashes on accessing history
		Gl_histinit( (char*)"-" );

		// prevent ROOT from exiting node
		SetReturnFromRun( kTRUE );
	}

	Bool_t NodeApplication::CreateNodeApplication()
	{
		if ( ! gApplication ) {
			int argc = 1;
			char** argv = new char*[ argc ];
			argv[ 0 ] = (char*)"node";

			gApplication = new NodeApplication( "rootJS", &argc, argv );
			delete[] argv;

			InitROOTGlobals();
			TObject::SetObjectStat(true);

			return kTRUE;
		}

		return kFALSE;
	}

	Bool_t NodeApplication::InitROOTGlobals()
	{
		if ( ! gBenchmark ) gBenchmark = new TBenchmark();
		if ( ! gStyle ) gStyle = new TStyle();

		if ( ! gProgName )              // should have been set by TApplication
			gSystem->SetProgname( "node" );

		gROOT->GetListOfCleanups()->Add(gApplication);

		return kTRUE;
	}

	void NodeApplication::RecursiveRemove(TObject *obj) {
		ObjectProxy::rootDesturcted(obj);
	}
}
