/*jshint expr: true*/
require('should');
var root = require('../index');


describe('void pointer', function() {
  describe('exposing', function() {
	  it('should have the property gMmallocDesc', function() {
		  (function() {
			  should.not.equal(root.gMmallocDesc, undefined);
		  }).should.not.throw();
	  });

	  it('property gMmallocDesc should be a string', function() {
		  root.gMmallocDesc.should.be.type('string');
		  console.log("gMmallocDesc points to address: " + root.gMmallocDesc);
	  });

	  it('gMmallocDesc should not be changeable', function() {
			  var gDebug = root.gMmallocDesc;

			  (function() {
				  root.gMmallocDesc = 'asdf';
			  }).should.throw();

			  root.gMmallocDesc.should.equal(gDebug);
	   });

	  it('should have the property gTQSender', function() {
		  (function() {
			  should.not.equal(root.gTQSender, undefined);
		  }).should.not.throw();
	  });

	  it('property gTQSender should be a string', function() {
		  root.gTQSender.should.be.type('string');
		  console.log("gTQSender points to address: " + root.gTQSender);
	  });

          it('gTQSender should not be changeable', function() {
			  var gDebug = root.gTQSender;

			  (function() {
				  root.gTQSender = 'asdf';
			  }).should.throw();

			  root.gTQSender.should.equal(gDebug);
	   });

  });
});
