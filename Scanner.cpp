#include "pch.h"
#include "Scanner.h"


int my_callback(
	YR_SCAN_CONTEXT* context,
	int message,
	void* message_data,
	void* user_data
) {
	switch (message)
	{
	case CALLBACK_MSG_RULE_MATCHING:
		((USER_DATA_CTX*)user_data)->match = TRUE;
		((USER_DATA_CTX*)user_data)->meta = ((YR_RULE*)message_data)->metas;
		break;
	}
	return CALLBACK_CONTINUE;
}


int compile_rule_ex(char* string, YR_RULES** rules, bool strict_escape_flag)
{
	YR_COMPILER* compiler = NULL;
	int result = ERROR_SUCCESS;

	if (yr_compiler_create(&compiler) != ERROR_SUCCESS)
	{
		mLogs->write("%s - %s - %s: yr_compiler_create fail", __FILE__, __FUNCTION__, __LINE__);
		goto _exit;
	}

	compiler->strict_escape = strict_escape_flag;

	if (yr_compiler_add_string(compiler, string, NULL) != 0)
	{
		result = compiler->last_error;
		mLogs->write("%s - %s - %s: yr_compiler_add_string fail", __FILE__, __FUNCTION__, __LINE__);
		goto _exit;
	}

	result = yr_compiler_get_rules(compiler, rules);

_exit:
	yr_compiler_destroy(compiler);
	return result;
}

int compile_rule(char* string, YR_RULES** rules)
{
	bool strict_escape_flag = false;
	return compile_rule_ex(string, rules, strict_escape_flag);
}

bool scan_memory_ex(char* rule, uint8_t* buffer, int size) {
	YR_RULES* rules;

	yr_initialize();

	if (compile_rule(rule, &rules) != ERROR_SUCCESS)
	{
		mLogs->write("%s - %s - %s: compile_rule fail", __FILE__, __FUNCTION__, __LINE__);
		exit(EXIT_FAILURE);
	}

	USER_DATA_CTX ctx;
	void* user_data = &ctx;

	int scan_result = yr_rules_scan_mem(rules, buffer, size, SCAN_FLAGS_NO_TRYCATCH, my_callback, user_data, 0);

	if (scan_result != ERROR_SUCCESS)
	{
		mLogs->write("%s - %s - %s: yr_rules_scan_mem fail", __FILE__, __FUNCTION__, __LINE__);
		exit(EXIT_FAILURE);
	}

	if (ctx.match) {
		mLogs->write("Found the harmful content: " + std::string(ctx.meta->string));
	}

	yr_rules_destroy(rules);
	yr_finalize();

	return ctx.match;
}

bool scan_memory(PBYTE buffer, int size) {
	return scan_memory_ex(my_rules, (uint8_t*)buffer, size);
}