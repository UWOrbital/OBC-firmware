import sys
import json
import argparse


class CallGraph(object):
    def __init__(self):
        self.funcs = {}

    def getFunction(self, name):
        if name not in self.funcs:
            self.funcs[name] = Function(name)
        return self.funcs[name]

    def getStackString(self, funcList):
        return " ".join([f.name + "/" + str(f.stack) for f in funcList])

    def getMaxStackSize(self):
        return max([c["maxSize"] for c in self.linearize()["children"]])

    def getStackSize(self, funcList):
        return sum([f.stack for f in funcList])

    def addCall(self, caller, callee):
        self.getFunction(caller).addCallee(self.getFunction(callee))
        self.getFunction(callee).addCaller(self.getFunction(caller))

    def linearizeNode(self, name, funcList, callStack = []):
        node = { "name": name, "children": [], "size": self.getStackSize(callStack), "callStack": self.getStackString(callStack) }
        if len(callStack) != len(set(callStack)):
            node["name"]      += " (recursion detected)"
            node["callStack"] += " (recursion detected)"
        else:
            for f in funcList:
                node["children"].append(self.linearizeNode(funcList[f].name, funcList[f].callee, callStack + [funcList[f]]))
        node["maxSize"] = max([c["maxSize"] for c in node["children"]] + [node["size"]])
        return node

    def linearize(self):
        return self.linearizeNode("", {k: v for k, v in self.funcs.items() if len(v.caller) == 0})

    def createJson(self, file):
        json.dump(self.linearize(), file, indent = 2)

    def createCsv(self, file, graph = None):
        if graph is None:
            return self.createCsv(file, self.linearize())

        if (len(graph["children"])):
            for child in graph["children"]:
                self.createCsv(file, child)
        else:
            file.write(str(graph["size"]) + ";" + graph["callStack"] + "\n")


class Function(object):
    def __init__(self, name):
        self.name   = name
        self.stack  = 0
        self.caller = {}
        self.callee = {}

    def setStackSize(self, size):
        self.stack = max(self.stack, size)

    def addCaller(self, caller):
        self.caller[caller.name] = caller

    def addCallee(self, callee):
        self.callee[callee.name] = callee


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = "Extract information about the stack usage and callgraphs of C/C++ programs using the standard GCC toolchain.")
    parser.add_argument("--stack-usage", default = "stack-usage-log.su",     help = "specifies the file which contains the output of GCCs -fstack-usage option")
    parser.add_argument("--callgraph",   default = "stack-usage-log.cgraph", help = "specifies the file which contains the output of GCCs -fdump-ipa-cgraph option")
    parser.add_argument("--csv",         default = None,                     help = "specifies the filename for CSV output")
    parser.add_argument("--json",        default = None,                     help = "specifies the filename for JSON output")

    args   = parser.parse_args()
    cgraph = CallGraph()

    with open(args.stack_usage, "r") as f:
        for line in f:
            cols = line.split('\t')
            text = cols[0].split(':')

            cgraph.getFunction(text[3]).setStackSize(int(cols[1]))

    with open(args.callgraph, "r") as f:
        for line in f:
            if line.find("@") != -1 and not line.startswith("  Aux: @"):
                name = line[0:line.find("/")]
            if line.startswith("  Calls:") and len(line) > 10:
                for callee in line[9:].split(" "):
                    if callee.find("/") != -1:
                        cgraph.addCall(name, callee[:callee.find("/")])

    print(cgraph.getMaxStackSize())

    if args.csv is not None:
        with open(args.csv, "w") as f:
            cgraph.createCsv(f)

    if args.json is not None:
        with open(args.json, "w") as f:
            cgraph.createJson(f)