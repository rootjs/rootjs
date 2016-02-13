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
});
