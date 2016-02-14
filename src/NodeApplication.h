#ifndef NODE_APPLICATION_H
#define NODE_APPLICATION_H

#include "TApplication.h"

namespace rootJS
{
	/**
	 * NodeApplication is used to handle ROOT GUIs
	 */
	class NodeApplication : public TApplication
	{
		public:
			/**
			 * Constructor for NodeApplication
			 * Accepts commandline arguments
			 *
			 * @param acn Application name
			 * @param argc number of parameters
			 * @param argv actual parameters
			 */
			NodeApplication(
			    const char* acn, Int_t* argc, char** argv);

			/**
			 * Destructor for NodeApplication
			 */
			virtual ~NodeApplication() { }

			/**
			 * Instamciates a new NdoeApplication and puts it to the right place (gApplicaiton)
			 */
			static Bool_t CreateNodeApplication( );

			/**
			 * This method should be used to initialize everything
			 * root needs to function prperly
			 */
			static Bool_t InitROOTGlobals();
	};
}

#endif /*NODE_APPLICATION_H*/
