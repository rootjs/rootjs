/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Functions', function() {
  describe('return values', function() {
	  it('boolean values should be returned even though they are false', function() {
		 /*
		  * FunctionProxy::call checked if cling set 0 as a return value.
		  * 0 was interpreted as a nullpointer, however false = 0 and numeric
		  * values can be 0, too
		  */
		 root.gSystem.InControl().should.equal(false);
	 });
  });
  describe('pure virtuals', function() {
	  it('should be possible to call gCling.ProcessLine', function() {
		  (function() {
			  root.gCling.ProcessLine("Printf(\"test\");");
		  }).should.not.throw();
	  });
  });
});
