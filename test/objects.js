/*jshint expr: true*/
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
	  it('should be possible to call methods asynchronously', function(done) {
		  var AsyncWorks = false;
		  root.gSystem.Exec("sleep 0.1", function(result) {
			  AsyncWorks.should.equal(true);
			  done();
		  });
		  AsyncWorks = true;
	  });
	  it('should be possible to get return values as a param of a callback function', function(done) {
		  root.gApplication.GetName(function(name) {
			  name.should.equal("TApplication");
			  done();
		  });
	  });
	  it('should be possible to run multiple functions asynchronously in parallel', function(done) {
		  var join = 5;
		  var cb = function() {
			  if(--join <= 0) {
				  done();
			  }
		  };
		  for(var i = 0; i < 5; i++) {
			  root.gSystem.Exec("sleep 0.1", cb);
		  }
	  });
  });
  describe('Constructor', function() {
	  it('should be possible to create a new TString', function(){
		  var testSring = new root.TString("test");
		  testSring.Data().should.equal("test");
	  });
  });
  describe('Operators', function() {
	  it('should be possible to use _equals', function() {
		  var t1 = new root.TString("test");
		  var t2 = new root.TString("test");

		  (t1 == t2).should.not.equal(true);
		  root._equals(t1, t2).should.equal(true);
	  });
	  it('should be possible to use _setAdd', function() {
		  var t1 = new root.TString("test");
		  var t2 = new root.TString("test");

		  t1._setAdd(t2);
		  t1.Data().should.equal("testtest");
	  });
	  it('should be possible to use _at', function() {
		  var t1 = new root.TString("test");

		  t1._at(2).should.equal('s');
		  (function() {
			  t1._at(10).should.equal('')
		  });
	  });
  })
});
