tableName = "SetItemEffectResource"
fileName = "db_setitemeffectresource.rdb"

fields = {
	{"set_id", INT32},
	{"set_part_id", INT32},
	{"text_id", INT32},
	{"tooltip_id", INT32},
	{"base_type_0", INT16},
	{"base_type_1", INT16},
	{"base_type_2", INT16},
	{"base_type_3", INT16},
	{"base_var1_0", DOUBLE},
	{"base_var1_1", DOUBLE},
	{"base_var1_2", DOUBLE},
	{"base_var1_3", DOUBLE},
	{"base_var2_0", DOUBLE},
	{"base_var2_1", DOUBLE},
	{"base_var2_2", DOUBLE},
	{"base_var2_3", DOUBLE},
	{"opt_type_0", INT16},
	{"opt_type_1", INT16},
	{"opt_type_2", INT16},
	{"opt_type_3", INT16},
	{"opt_var1_0", DOUBLE, flag=BIT_FLAG, opt={ "buff_flags_91" }},
	{"opt_var1_1", DOUBLE, flag=BIT_FLAG, opt={ "buff_flags_91" }},
	{"opt_var1_2", DOUBLE, flag=BIT_FLAG, opt={ "buff_flags_91" }},
	{"opt_var1_3", DOUBLE, flag=BIT_FLAG, opt={ "buff_flags_91" }},
	{"opt_var2_0", DOUBLE, flag=BIT_FLAG, opt={ "buff_flags_91" }},
	{"opt_var2_1", DOUBLE, flag=BIT_FLAG, opt={ "buff_flags_91" }},
	{"opt_var2_2", DOUBLE, flag=BIT_FLAG, opt={ "buff_flags_91" }},
	{"opt_var2_3", DOUBLE, flag=BIT_FLAG, opt={ "buff_flags_91" }},
	{"effect_id", INT32}
}