/*jshint expr: true*/
require('should');
var root = require('../index');


describe('Namespaces', function() {
  describe('exposing', function() {
	  it('should have the property ROOT.Detail.TSchemaRuleSet', function() {
		  (function() {
			  should.not.equal(root.ROOT.Detail.TSchemaRuleSet, undefined);
		  }).should.not.throw();
	  });
	  it('should have the property ROOT.Detail', function() {
		  (function() {
			  should.not.equal(root.Detail, undefined);
		  }).should.not.throw();
	  });
	  it('should have the property ROOT.TStreamerInfoActions', function() {
		  (function() {
			  should.not.equal(root.TStreamerInfoActions, undefined);
		  }).should.not.throw();
	  });
	  it('should have the property TString', function() {
		  (function() {
			  should.not.equal(root.TString, undefined);
		  }).should.not.throw();
	  });
  });
});
