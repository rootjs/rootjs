/*jshint expr: true*/
require('should');
var root = require('../index');

describe('enums', function() {

  describe('exposure', function() {
	  it('there should be a global enum EColorPalette', function() {
		  (function() {
			  should.not.equal(root.EColorPalette, undefined);
		  }).should.not.throw();
	  });

    it('there should be a an enum EStatusBits in class TObject', function() {
		  (function() {
			  should.not.equal(root.TObject.EStatusBits, undefined);
		  }).should.not.throw();
	  });
  });

  describe('enum assignment', function() {
	  it('global enum EColorPalette should not be changeable', function() {
			  var palette = root.EColorPalette;

			  (function() {
				  root.EColorPalette = root.TObject.EStatusBits;
			  }).should.throw();

			  root.EColorPalette.should.equal(palette);
	   });


    it('enum TObject::EStatusBits should not be changeable', function() {
			  var bits = root.TObject.EStatusBits;

			  (function() {
				  root.TObject.EStatusBits = root.EColorPalette;
			  }).should.throw();

			  root.TObject.EStatusBits.should.equal(bits);
	   });
  });

  describe('enum constant assignment', function() {
     it('enum constant kCoffee of global enum EColorPalette should not be changeable', function() {
       var coffee = root.EColorPalette.kCoffee;

       (function() {
         root.EColorPalette.kCoffee = 42;
       }).should.throw();

       root.EColorPalette.kCoffee.should.equal(coffee);
     });

     it('enum constant kCanDelete of enum TObject::EStatusBits should not be changeable', function() {
        var del = root.TObject.EStatusBits.kCanDelete;

        (function() {
         root.TObject.EStatusBits.kCanDelete = 42;
        }).should.throw();

        root.TObject.EStatusBits.kCanDelete.should.equal(del);
     });
  describe('Note that the following tests on enum constant kCoffee in global scope should fail because kCoffee is constant! Since ROOT does not provide information about the read only access it is possible to abuse kCoffee for test purposes.', function() {

     it('should not be possible to assign a non number object to kCoffee', function() {
        var old = root.kCoffee;

        (function() {
         root.kCoffee = "asdf";
       }).should.throw();

        root.kCoffee.should.equal(old);
     });

     it('should not be possible to assign an encapsulated object of different type to kCoffee', function() {
        var old = root.kCoffee;

        (function() {
         root.kCoffee = new root.TString("asdf");
       }).should.throw();

        root.kCoffee.should.equal(old);
     });

     it('should not be possible to assign an object of different type to kCoffee', function() {
        var old = root.kCoffee;

        function dummy(name) {
          this.name = name;
        }
        var obj = new dummy("test");

        (function() {
         root.kCoffee = obj;
       }).should.throw();

        root.kCoffee.should.equal(old);
     });

     it('should not be possible to assign a number not present in EColorPalette to kCoffee', function() {
        var old = root.kCoffee;

        (function() {
         root.kCoffee = 42;
       }).should.throw();

        root.kCoffee.should.equal(old);
     });

     it('should be possible to assign a number present in EColorPalette to kCoffee', function() {
        var old = root.kCoffee;

        (function() {
         root.kCoffee = root.EColorPalette.kPearl;
       }).should.not.throw();

        root.kCoffee.should.not.equal(old);
        root.kCoffee = old;
     });
    });
  });

});
