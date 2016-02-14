/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Dynamic Loading', function() {
    this.timeout(10000);
    describe('interface', function () {
        it('should be possible to call loadlibrary to load MathCore', function () {
            (function () {
                root.loadlibrary("libMathCore.so");
            }).should.not.throw();
        });

        describe('accessing', function () {
            it('ROOT.Math should not be undefined anymore', function () {
                root.ROOT.Math.should.not.be.undefined()
            });

            it('pi should be equal to 3.141592653589793', function () {
                root.ROOT.Math.Pi().should.be.equal(3.141592653589793);
            });
        });
    });
});
