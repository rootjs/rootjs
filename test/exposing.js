require('should');
var root = require('../index');


describe('Exposing', function () {

    it('should have a property named TStreamerInfoActions', function () {
        root.TStreamerInfoActions.should.not.be.undefined()
    });
    it('should have a property named ROOT.Detail', function () {
        root.ROOT.Detail.should.not.be.undefined()
    });

    it('should have a property named TString', function () {
        (function () {
            should.not.equal(root.TString, undefined);
        }).should.not.throw();
    });
    it('should have a property named ROOT.Detail.TSchemaRuleSet', function () {
        root.ROOT.Detail.TSchemaRuleSet.should.not.be.undefined()
    });
    it('should be possible to create instance of ROOT.TSchemaRule.TSources', function () {
        var tc = new root.ROOT.TSchemaRule.TSources();
        tc.should.not.be.undefined();
    });
    it('should return "" when calling GetName() on a ROOT.TSchemaRule.TSources instance', function () {
       var tc = new root.ROOT.TSchemaRule.TSources();
       tc.GetName().should.equal("");
    })

});