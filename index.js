module.exports = require('./build/Debug/rootjs.node');

var uicallback = function() {
	module.exports.gSystem.ProcessEvents();
	setTimeout(uicallback, 20);
}
uicallback();
