/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Objects', function() {
  describe('methods', function() {
	  it('should not be possible to call gSystem.SizeOf without the corect args', function() {
		  (function() {
			  root.gSystem.Sizeof();
		  }).should.not.throw();
		  (function() {
			  root.gSystem.SizeOf(true);
		  }).should.throw();
		  (function() {
			  root.gSystem.SizeOf("test");
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
	  it('should be possible to call static methods asynchronously', function(done) {
		  var t1 = new root.TString("test");
		  var t2 = new root.TString("test");

		  root._equals(t1, t2, function(result) {
			  result.should.equal(true);
			  done();
		  });
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
  describe('Attributes', function() {
	  it('should be possible to read public attributes', function() {
		  var test = new root.TArrayL(1)
		  test.fArray.should.equal(0);
	  });
	  xit('should be possible to write public attributes', function() {
		  var test = new root.TArrayL(1)
		  test.fArray.should.equal(0);
		  test.fArray = 1;
		  test.fArray.should.equal(1);
	  });
  });
  describe('Constructor', function() {
	  it('should be possible to create a new TString', function(){
		  var testSring = new root.TString("test");
		  testSring.Data().should.equal("test");
	  });
	  it('should not be possible to create a new TString without the new operator', function(){
		 (function() {
			 var testSring = root.TString("test");
		 }).should.throw();
	 });
	 it('should not be possible to create a new TString without passing matching arguments', function(){
		(function() {
			var testSring = new root.TString(true);
		}).should.throw();
	});
	  it('should be possible to create a new TString asynchronously', function(done){
		  var asyncBarriere = false;
		  new root.TString("test", function(testString) {
			   testString.Data().should.equal("test");
			   asyncBarriere.should.equal(true);
			   done();
		  });
		  asyncBarriere = true;
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
  });
  describe('static', function()  {
	  it('should be possible to read static attributes', function() {
		  root.TApplication.kExit.should.equal(1);
	  });
	  it('should not be possible to write static const attributes', function() {
		  (function() {
			  root.TApplication.kExit = 2;
		  }).should.throw();
	  });
  });
});
