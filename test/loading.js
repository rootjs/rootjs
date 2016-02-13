/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Dynamic Loading', function() {
    describe('interface', function () {
        it('should be possible to call loadlibrary', function () {
            (function () {
                root.loadlibrary("");
            }).should.not.throw();
        });
    });
});
