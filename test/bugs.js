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
  describe('#30', function() {
	  it('should be possible to pass objects', function() {
		  var test = new root.TString("test");
		  var test2 = new root.TString(test);
		  test2.Data().should.equal("test");
	  });
	  it('should be possible to pass pointers to objects as params', function() {
		  var object = new root.TObject();
		  var browser = new root.TBrowser();
		  (function() {
			  browser.Add(object);
		  }).should.not.throw();
	  });
  });
});
