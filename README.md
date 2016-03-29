**Jenkins**: *[![Build Status](http://jnugh.de:8080/job/RootJS/badge/icon)](http://jnugh.de:8080/job/RootJS/), Sir!*
# rootJS
Node.js Bindings for ROOT6 implementation (http://wiki.scc.kit.edu/lsdf/index.php/Node.js_Bindings_for_ROOT6)
# Docs
Check this http://rootjsdocs.jnugh.de/ for the current documentation.
Documentation is generated by Hudson, so this link should always point to a recent version.
# Installation
rootJS is in the main npm package repository - all you have to do is have ROOT6 installed, then run `npm install rootjs`.
# Testing
Simply run `npm test` after installing rootJS in development mode.

We currently use a mocha testsuite but might add something to (unit-)test C++ code directly.

#Dependencies
* You need to have ROOT installed (root-config needs to be available in your PATH)
* You need libuv, on Ubuntu you can use `sudo apt-get install libuv1-dev` to install it

#Usage
You can use rootJS in a node shell by using require on the provided index.js file, e.g.:
```javascript
var root = require('./index.js');
```

Afterwards you have access to root's functionality through the required `root` object which copies its structure from ROOT.

```javascript
> root.gProgName //You can access globals
'node'
> root.Compress("compress this text") //You can call global functions
'compressthistext'
> var browser = new root.TBrowser() //You can create new objects (even GUI elements)
undefined
> browser.SetName("test") //And run its methods
'165781144'
> browser.GetName()
'test'
```

##Load libraries
You can easily load other ROOT libraries whenever needed, just use `loadLibrary`:
```javascript
>root.loadlibrary("libMathCore.so") //Loads math core
0
>root.ROOT.Math.Pi()
3.141592653589793 //Verified!
```
Calling ROOT's `gSystem.load("libName.so")`to load a library is highly discouraged and may result in unexpected behaviour.

If neccessary a refresh of the exported functions can be done through `refreshExports` 
```javascript
>root.refreshExports() //Checks for additions to the gClassTable and exposes them
undefined
```
##Callbacks
We support async code execution on every method call, just add a callback:
```javascript
> root.gSystem.Exec("sleep 1", function() {
	console.log("this is called in the callback")
});
>console.log("Async called");
Async called // The second log is shown first because the other one will be called after "sleep 1"
undefined
> this is called in the callback
```
This will be usefull when processing huge amounts of data while setting up other parts of the environment.

##Operators
C++ operators like "==" are supported, these operators are mapped on objects or in the global root object
```javascript
> var t1 = new root.TString("test")
undefined
> var t2 = new root.TString("test")
undefined
> t1 == t2
false
> root._equals(t1, t2) // C++ == operator
true

> t1._at(1) //C++ []-operator
'e'

t1._call(2, 2).Data() //C++ ()-operator
'st'

t1._setAdd(" test2") //C++ += operator
undefined
> t1.Data()
'test test2'

//All implemented operators are prefixed with "_"
