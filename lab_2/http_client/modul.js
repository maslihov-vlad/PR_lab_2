import * as s from '/modules/my-module.js';
var proto = "RAW IP";

function set_proto(proto_)
{
	proto = proto
	document.getElementById("status").textContent="PROTOCOL: "+proto_;
}