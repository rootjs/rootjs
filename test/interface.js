require('should');

describe('Interface', function() {
  describe('require', function() {
    it('should return an object', function() {
      var root = require('../index');
      root.should.be.type('object');
    });
  });
});
