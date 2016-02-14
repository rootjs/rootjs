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
	  xit('should be possible to pass pointers to objects as params', function(done) { //Test is unstable...
		  var object = new root.TObject();
		  var browser = new root.TBrowser();
		  (function() {
			  browser.Add(object);
		  }).should.not.throw();
		  setTimeout(function() {
			  //browser.Destructor();
			  /* When we do not run this we get a segfault because the initialization is not complete before the test is done and
			   * deletion of a TBrowser when it's not fully loaded leads to a crash */
			   done();
		  }, 200);
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
