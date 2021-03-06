/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Interface', function() {
  describe('require', function() {
    it('should return an object', function() {
      root.should.be.type('object');
    });
  });

  describe('globals', function() {
      describe('numbers', function() {
          it('should have a property named kMaxChar', function() {
              root.kMaxChar.should.not.be.undefined();
          });

		  it('should have kMaxChar which equals 128', function(){
              root.kMaxChar.should.equal(128);
          });

		  it('should save gDebug as an integer (drop decimal values)', function() {
			  var gDebug = root.gDebug;
			  root.gDebug = 4;
			  root.gDebug.should.equal(4);

			  root.gDebug = 5.4;
			  root.gDebug.should.equal(5);

			  root.gDebug = 5.9;
			  root.gDebug.should.equal(5);

			  root.gDebug = gDebug;
		  });

		  it('should overflow silently', function() {
			  var gDebug = root.gDebug;
			  root.gDebug = 1e+15;
			  root.gDebug.should.not.equal(1e+15);
			  root.gDebug = gDebug;
		  });

		  it('should not accept values that are not numeric', function() {
			  var gDebug = root.gDebug;

			  (function() {
				  root.gDebug = 'asdf';
			  }).should.throw();

			  (function() {
				  root.gDebug = true;
			  }).should.throw();

			  (function() {
				  root.gDebug = {};
			  }).should.throw();

			  (function() {
				  root.gDebug = [];
			  }).should.throw();

			  root.gDebug.should.equal(gDebug);
		  });

          it('should have a property named kInitTreatAsZero', function() {
              root.kInitTreatAsZero.should.not.be.undefined();
          });

          it('should have kInitTreatAsZero which equals 1e-12', function(){
              root.kInitTreatAsZero.should.equal(1e-12);
          });

		  it('should have kInitTreatAsZero which equals 1e-12', function(){
              root.kInitTreatAsZero.should.equal(1e-12);
          });

		  it('should fail to write to const globals', function() {
			  var oldValue = root.kInitTreatAsZero;
			  (function(){
				  root.kInitTreatAsZero = 0;
			  }).should.throw();
			  root.kInitTreatAsZero.should.equal(oldValue);
		  });
      });
	  describe('strings', function() {
		  it('should have the gProgName "node"', function() {
			  root.gProgName.should.equal('node');
		  });
		  it('should consider char pointers to be immutable', function() {
			  var oldValue = root.gProgName;
			  (function(){
				  root.gProgName = 'testImmutable';
			  }).should.throw();
			  root.gProgName.should.equal(oldValue);
		  });
	  });
	  describe('booleans', function() {
		  it('should have kTRUE which should be true', function() {
			  root.kTRUE.should.equal(true);
		  });
		  it('should have kFALSE which should be false', function() {
			  root.kFALSE.should.equal(false);
		  });
		  it('should fail to change the value of kFALSE', function() {
			  (function() {
				  root.kFALSE = true;
			  }).should.throw();
			  root.kFALSE.should.equal(false);
		  });
		  it('should be possible to set a boolean', function() {
			  //There is no global bool which is writable :(
			  root.gCling.ProcessLine("bool test = true");
			  root.refreshExports();
			  root.test.should.equal(true);
			  root.test = false;
			  root.test.should.equal(false);
			  root.test = new Boolean(true);
			  root.test.should.equal(true);
		  });
		  it('should not be possible to store something else then a boolean in a boolean variable', function () {
			  root.gCling.ProcessLine("bool test2 = true");
			  root.refreshExports();
			  (function() {
				  root.test2 = "asdf";
			  }).should.throw();
			  root.test2.should.equal(true);
		  });
	  });
	  describe('functions', function() {
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
		  it('should be possible to call functions which return non-scalar values', function() {
			  root.ToUpper(new root.TString("test")).Data().should.equal("TEST");
		  })
	  });
  });
});
