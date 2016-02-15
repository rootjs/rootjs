/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Bugs', function() {
  describe('#15', function() {
	  it('should be possible to have non pointer references to returned objects from function calls', function() {
		 var tstring = new root.TString("test");
		 tstring.Copy().Data().should.equal("test");
	 });
	});
	describe('#35', function () {
		it('should not return null from ROOT::GetClass', function () {
			var ts = root.ROOT.GetROOT();
				ts.should.not.be.type('undefined')
		});
	});
  	describe('#30', function() {
	  it('should be possible to pass objects', function() {
		  var test = new root.TString("test");
		  var test2 = new root.TString(test);
		  test2.Data().should.equal("test");
	  });
	  it('should be possible to pass pointers to objects as params', function() { //Test is unstable...
		  (function() {
			  root.ROOT.GetROOT().SetApplication(root.gApplication);
		  }).should.not.throw()
	  });
  });
  describe('#51', function() {
	  it('should be possible to use _set on a TString with a js string as parameter', function() {
		  var str = new root.TString("");
		  str._set("test");
		  str.Data().should.equal("test");
	  });
	  it('should be possible to use _set on a TString with a js string that is a char', function() {
		  var str = new root.TString("");
		  str._set("t");
		  str.Data().should.equal("t");
	  });
  });
  describe('constants', function() {
	  it('constants should really be constant!', function() {
		  (function() {
			  root.kFALSE = true;
		  }).should.throw();
		  root.kFALSE.should.equal(false);
	 });
  });
});
