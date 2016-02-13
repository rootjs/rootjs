/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Dynamic Loading', function() {
    describe('interface', function () {
        it('should be possible to call loadlibrary to load libASImageGui', function () {
            (function () {
                root.loadlibrary("libASImageGui.so").should.equal(0);
            }).should.not.throw();
        });
        it('should be possible to trigger a refresh of the exported functions', function (){
            (function () {
                root.refreshExports();
            }).should.not.throw();
        });
    });
});
