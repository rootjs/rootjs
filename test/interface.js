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

		  xit('should fail to write to const globals', function() {
			  (function(){
				  root.kInitTreatAsZero = 0;
			  }).should.throw();
		  });
      });
  });
});
