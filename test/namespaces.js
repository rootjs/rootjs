require('should');
var root = require('../index');


describe('Namespaces', function() {
  describe('exposing', function() {
	  it('should have the property ROOT.Detail.TSchemaRuleSet', function() {
		  (function() {
			  should.not.equal(root.ROOT.Detail.TSchemaRuleSet, undefined);
		  }).should.not.throw();
	  });
  });
});
