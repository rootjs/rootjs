/*jshint expr: true*/
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
	  it('should be possible to call a function with a callback that is called when the root code has been processed', function(done) {
		  root.Compress('these are multiple words', function(result) {
			  result.should.equal('thesearemultiplewords');
			  done();
		  });
		  root.Compress('these are multiple words').should.equal('thesearemultiplewords');
	  });
	  it('should be possible to pass doubles as params', function() {
		  var test = new root.TString("test");
		  test.Data().should.equal("test");
		  (function() {
			  test._setAdd(0.01);
		  }).should.not.throw();
		  test.Data().should.equal("test0.01");
	  });
  });
  describe('parameter types', function() {
	  it('should be possible to pass objects', function() {
		  var test = new root.TString("test");
		  var test2 = new root.TString(test);
		  test2.Data().should.equal("test");
	  });
  });
});
