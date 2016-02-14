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
        it('should be possible to call reloadLibrary', function () {
            (function () {
            root.refreshExports()
            }).should.not.throw();
        });
    });
        describe('accessing', function () {
            it('should be able to access TSpectrum2Painter', function () {
                root.TSpectrum2Painter.should.not.be.undefined()
            });
            it('should be possible to get TSPectrum2Painter\'s name',function () {
                root.loadlibrary("libSpectrumPainter.so");
                root.TSpectrum2Painter.Class_Name().should.equal("TSpectrum2Painter")
            });

            it('pi should be equal to 3.141592653589793', function () {
                root.ROOT.Math.Pi().should.be.equal(3.141592653589793);
            });
        });
    describe('unloading', function () {
        it('should be possible call for unloading of libSpectrumPainter.so', function () {
            (function () {
                root.unloadlibrary("libSpectrumPainter.so");
            }).should.not.throw();
        });

    });
});

