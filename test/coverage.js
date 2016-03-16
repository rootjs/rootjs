require('should');
var root = require('../index');

var instanceCache = { //Also blacklist
	TF1Convolution: false, //Constructor crashes when passing two empty TF1 objects, skip this
	TGFileInfo: false, //Depends on a nullptr
	TGClient: false, //Don't want to connect to a display sever...
	TGraphDelaunay2D: false, //Needs a valid 2d graph, segfaults without
	THnSparseArrayChunk: false, //Doc: "don't use directly.    "
	TROOT: false, //Simply not allowed (new is privte)
	TGuiBuilder: false, //Does not work headless (Jenkins)
	'TStreamerInfoActions::TConfiguredAction': false, //Would be instantiated with a malformed TConfigureAction and therefore crash
	'TRootCanvas': false, //GUI on Jenkins...
	'TInspectCanvas': false, //Seems to crash randomly on Jenkins
	'TRedirectOutputGuard': false //just for resuming test on jenkins
};

var lastError = '';

function instantiate(currentClass, reflectionName) {
	var className = currentClass.GetName();

	if(instanceCache[className] !== undefined) {
		if(instanceCache[className] === 'PENDING') {
			lastError = 'cyclicDependent';
			instanceCache[className] = false;
			return false;
		}
		return instanceCache[className];
	}
	instanceCache[className] = 'PENDING';

	var parts = className.split('::');
	var currentObj = root;
	var lastPart = '';

	for(var j = 0; j < parts.length; j++) {
		if(!currentObj[parts[j]]) {
			currentObj = undefined;
			break;
		} else {
			lastPart = parts[j];
			currentObj = currentObj[parts[j]];
		}
	}


	if(typeof(currentObj) != 'function') {
		lastError = 'noConstructor';
		instanceCache[className] = false;
		return false;
	}

	//wrap constructor to support arbitrary count of params
	var params = [""].concat(getValidConstructorParams(lastPart, currentClass, reflectionName));
	var constructor = Function.prototype.bind.apply(currentObj, params);

	var result = new constructor();
	instanceCache[className] = result;
	return result;
}

function getValidConstructorParams(name, classRef, reflectionName) {
	root.gCling.ProcessLine("currentConstructor = (TMethod *)" + reflectionName + "->GetListOfAllPublicMethods()->FindObject(\"" + name + "\");");
	root.gCling.ProcessLine("constructorExists = currentConstructor != nullptr;");

	if(!root.constructorExists) {
		return false;
	}

	root.refreshExports();
	var methodReflection = root.currentConstructor;
	if(!methodReflection) {
		return [];
	}
	root.gCling.ProcessLine("currentArgList = currentConstructor->GetListOfMethodArgs();");
	root.gCling.ProcessLine("currentArg = nullptr;");

	var results = [];

	for(var i = 0; i < root.currentArgList.GetSize(); i++) {
		root.gCling.ProcessLine("currentArg = (TMethodArg*)currentArgList->At(" + i + ");");

		switch(root.currentArg.GetTypeName()) {
			case 'vector<double>':
				results.push(new root['vector<double>']());
				break;
			case 'unsigned int':
			case 'UInt_t':
			case 'Int_t':
			case 'Long_t':
			case 'Atom_t':
			case 'Double_t':
			case 'double':
			case 'int':
			case 'uint64_t':
			case 'Long64_t':
			case 'ULong_t':
			case 'Pixel_t':
			case 'GContext_t':
			case 'FontStruct_t':
				results.push(1);
				break;
			case 'Bool_t':
			case 'bool':
				results.push(true);
				break;
			case 'char':
				results.push('c');
				break;
			case 'ROOT::Fit::BinData::ErrorType':
				results.push(root.ROOT.Fit.BinData.ErrorType.kNoError);
				break;
			default:
				var type = root.currentArg.GetTypeName();
				root.gCling.ProcessLine("reflectionExists = gClassTable->GetDict(\"" + type + "\") != nullptr;");

				if(root.reflectionExists) {
					root.gCling.ProcessLine("paramClassReflection = (TClass *)gClassTable->GetDict(\"" + type + "\")();");
					var instance = instantiate(root.paramClassReflection, "paramClassReflection");
					results.push(instance);
				} else {
					console.warn("Unknown parameter: " + root.currentArg.GetTypeName());
				}
		}

	}

	return results;
}


describe('Coverage', function() {
	it('Should be possible to call the constructor of every class', function() {
	 	this.timeout(10000000);
		var workingCount = 0;
		var allCount = root.gClassTable.Classes();
		var errors = {};
		root.gCling.ProcessLine("TClass *currentClass;");
		root.gCling.ProcessLine("TClass *paramClassReflection;");
		root.gCling.ProcessLine("bool reflectionExists;");
		root.gCling.ProcessLine("bool classValid;");
		root.gCling.ProcessLine("bool constructorExists;");

		root.gCling.ProcessLine("TMethod *currentConstructor;");
		root.gCling.ProcessLine("TList *currentArgList;");
		root.gCling.ProcessLine("TMethodArg *currentArg;");

		root.refreshExports();

		for(var i = 0; i < allCount; i++) {
			root.gCling.ProcessLine("currentClass = gClassTable->GetDict(gClassTable->At(" + i + "))();");
			console.log(root.currentClass.GetName());
			try {
				var test = instantiate(root.currentClass, "currentClass");
				if(!test) {
					if(!errors[lastError])
						errors[lastError] = 0;
					errors[lastError]++;
				} else {
					workingCount++;
				}
			} catch (e) {
				if(!errors[e.message]) {
					errors[e.message] = 0;
				}
				errors[e.message]++;
			}
			console.log(root.currentClass.GetName() + 'END');
		}

		console.warn(workingCount + ' constructors of ' + allCount + ' available classes have been called', errors);
		instanceCache = {}; //Destructors :)
		global.gc();
	});
});
