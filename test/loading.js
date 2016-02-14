/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Dynamic Loading', function() {
    describe('interface', function () {
        it('should be possible to call loadlibrary to load MathCore', function () {
            (function () {
              root.loadlibrary("libMathCore.so").should.equal(0);
            }).should.not.throw();
        });
        it('should be possible to call stuffi', function () {
               root.ROOT.Math.should.not.be.undefined()

        });
    });
});
