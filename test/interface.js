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

          it('should have a property named kInitTreatAsZero', function() {
              root.kInitTreatAsZero.should.not.be.undefined();
          });

          it('should have kInitTreatAsZero which equals 1e-12', function(){
              root.kInitTreatAsZero.should.equal(1e-12);
          });
      });
  });
});
