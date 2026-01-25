"use strict";
exports.__esModule = true;
exports.type_to_cell_type = exports.type_to_matrix_type = exports.binaryOpType = void 0;
var binaryOpType = function (left_type, right_type) {
    if (left_type == right_type) {
        return left_type;
    }
    else if (left_type == 'complex' || right_type == 'complex') {
        return 'complex';
    }
    else if (left_type == 'double' || right_type == 'double') {
        return 'double';
    }
    else if (left_type == 'bool' || left_type == 'unknown') {
        return right_type;
    }
    else if (right_type == 'bool' || right_type == 'unknown') {
        return left_type;
    }
    else {
        return 'unknown';
    }
};
exports.binaryOpType = binaryOpType;
exports.type_to_matrix_type = [
    { type: "integer", matrix_type: 0 },
    { type: "int", matrix_type: 0 },
    { type: "double", matrix_type: 1 },
    { type: "complex", matrix_type: 2 },
    { type: "char", matrix_type: 3 }
];
exports.type_to_cell_type = [
    { type: "integer", cell_type: 0, cell_val: "ival" },
    { type: "int", cell_type: 0, cell_val: "ival" },
    { type: "double", cell_type: 1, cell_val: "dval" },
    { type: "complex", cell_type: 2, cell_val: "cval" },
    { type: "char", cell_type: 3, cell_val: "chval" }
];
//# sourceMappingURL=customTypes.js.map