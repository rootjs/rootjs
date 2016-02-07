require('should');
var root = require('../index');


describe('Functions', function() {
  describe('overloading', function() {
	  it('should not be possible to call Printf without the corect args', function() {
		  (function() {
			  root.Printf();
		  }).should.throw();
		  (function() {
			  root.Printf(true);
		  }).should.throw();
		  (function() {
			  root.Printf("test");
		  }).should.not.throw();
	  });
	  it('should return a number when calling Hash on a string', function() {
		  root.Hash("TestString").should.be.type('number');
	  });
	  it('should be possible to call a function that returns a char*', function() {
		  root.Compress('these are multiple words').should.equal('thesearemultiplewords');
	  });
  });
});