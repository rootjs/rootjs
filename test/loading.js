/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Dynamic Loading', function() {
    this.timeout(10000);
    describe('interface', function () {
        it('should be possible to call loadlibrary to load libSpectrumPainter', function () {
            (function () {
                root.loadlibrary("libSpectrumPainter.so");
            }).should.not.throw();
        });
        it('should be possible to call reloadExports', function () {
            (function () {
            root.refreshExports()
            }).should.not.throw();
        });
        describe('accessing', function () {
            it('should be able to access TSpectrum2Painter', function () {
                root.TSpectrum2Painter.should.not.be.undefined()
            });

            it('pi should be equal to 3.141592653589793', function () {
                root.ROOT.Math.Pi().should.be.equal(3.141592653589793);
            });
        });
    });
});
