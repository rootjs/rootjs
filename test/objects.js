require('should');
var root = require('../index');


describe('Objects', function() {
  describe('methods', function() {
	  it('should not be possible to call gSystem.Dump without the corect args', function() {
		  (function() {
			  root.gSystem.Dump();
		  }).should.not.throw();
		  (function() {
			  root.gSystem.Dump(true);
		  }).should.throw();
		  (function() {
			  root.gSystem.Dump("test");
		  }).should.throw();
	  });
	  it('should have gSystem.ProcessEvents returning a boolean value', function() {
		  root.gSystem.ProcessEvents().should.be.type('boolean');
	  });
	  it('should be possible to call virtual methods', function() {
		  (function() {
			  root.gBenchmark.Start("test");
		  }).should.not.throw();
	  });
  });
});
