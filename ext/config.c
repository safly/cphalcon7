
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#include "pconfig.h"
#include "config/exception.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/hash.h"
#include "kernel/fcall.h"
#include "kernel/object.h"
#include "kernel/operators.h"
#include "kernel/array.h"

#include "internal/arginfo.h"

/**
 * Phalcon\Config
 *
 * Phalcon\Config is designed to simplify the access to, and the use of, configuration data within applications.
 * It provides a nested object property based user interface for accessing this configuration data within
 * application code.
 *
 *<code>
 *	$config = new Phalcon\Config(array(
 *		"database" => array(
 *			"adapter" => "Mysql",
 *			"host" => "localhost",
 *			"username" => "scott",
 *			"password" => "cheetah",
 *			"dbname" => "test_db"
 *		),
 *		"phalcon" => array(
 *			"controllersDir" => "../app/controllers/",
 *			"modelsDir" => "../app/models/",
 *			"viewsDir" => "../app/views/"
 *		)
 * ));
 *</code>
 *
 */
zend_class_entry *phalcon_config_ce;

PHP_METHOD(Phalcon_Config, __construct);
PHP_METHOD(Phalcon_Config, val);
PHP_METHOD(Phalcon_Config, offsetExists);
PHP_METHOD(Phalcon_Config, get);
PHP_METHOD(Phalcon_Config, offsetGet);
PHP_METHOD(Phalcon_Config, offsetSet);
PHP_METHOD(Phalcon_Config, offsetUnset);
PHP_METHOD(Phalcon_Config, merge);
PHP_METHOD(Phalcon_Config, toArray);
PHP_METHOD(Phalcon_Config, count);
PHP_METHOD(Phalcon_Config, __wakeup);
PHP_METHOD(Phalcon_Config, __set_state);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, arrayConfig)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config_val, 0, 0, 1)
	ZEND_ARG_INFO(0, arrayConfig)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config_get, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, defaultValue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config_merge, 0, 0, 1)
	ZEND_ARG_INFO(0, config)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config_toarray, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_config___wakeup, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_config_method_entry[] = {
	PHP_ME(Phalcon_Config, __construct, arginfo_phalcon_config___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_Config, val, arginfo_phalcon_config_val, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, offsetExists, arginfo_arrayaccess_offsetexists, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, get, arginfo_phalcon_config_get, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, offsetGet, arginfo_arrayaccess_offsetget, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, offsetSet, arginfo_arrayaccess_offsetset, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, offsetUnset, arginfo_arrayaccess_offsetunset, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, merge, arginfo_phalcon_config_merge, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, toArray, arginfo_phalcon_config_toarray, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, count, arginfo_countable_count, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, __wakeup, arginfo_phalcon_config___wakeup, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Config, __set_state, arginfo___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_MALIAS(Phalcon_Config, __get, offsetGet, arginfo___get, ZEND_ACC_PUBLIC)
	PHP_MALIAS(Phalcon_Config, __set, offsetSet, arginfo___set, ZEND_ACC_PUBLIC)
	PHP_MALIAS(Phalcon_Config, __isset, offsetExists, arginfo___isset, ZEND_ACC_PUBLIC)
	PHP_MALIAS(Phalcon_Config, __unset, offsetUnset, arginfo___unset, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\Config initializer
 */
PHALCON_INIT_CLASS(Phalcon_Config){

	PHALCON_REGISTER_CLASS(Phalcon, Config, config, phalcon_config_method_entry, 0);

	zend_class_implements(phalcon_config_ce, 2, zend_ce_arrayaccess, spl_ce_Countable);

	return SUCCESS;
}

/**
 * Phalcon\Config constructor
 *
 * @param array $arrayConfig
 */
PHP_METHOD(Phalcon_Config, __construct){

	zval *array_config = NULL;

	phalcon_fetch_params(0, 0, 1, &array_config);

	/** 
	 * Throw exceptions if bad parameters are passed
	 */
	if (array_config && Z_TYPE_P(array_config) != IS_ARRAY && Z_TYPE_P(array_config) != IS_NULL) {
		PHALCON_THROW_EXCEPTION_STRW(phalcon_config_exception_ce, "The configuration must be an Array");
		return;
	}

	if (array_config && Z_TYPE_P(array_config) == IS_ARRAY) {
		PHALCON_CALL_SELFW(NULL, "val", array_config);
	}
}

/**
 * Sets values
 *
 * @param array $arrayConfig
 */
PHP_METHOD(Phalcon_Config, val){

	zval *array_config, *value;
	zend_string *str_key;
	ulong idx;

	phalcon_fetch_params(0, 1, 0, &array_config);

	/** 
	 * Throw exceptions if bad parameters are passed
	 */
	if (Z_TYPE_P(array_config) != IS_ARRAY) {
		PHALCON_THROW_EXCEPTION_STRW(phalcon_config_exception_ce, "The configuration must be an Array");
		return;
	}

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array_config), idx, str_key, value) {
		zval key, instance;
		if (str_key) {
			ZVAL_STR(&key, str_key);
		} else {
			ZVAL_LONG(&key, idx);
		}

		if (Z_TYPE_P(value) == IS_ARRAY) {
			object_init_ex(&instance, phalcon_config_ce);
			PHALCON_CALL_METHODW(NULL, &instance, "__construct", value);
			PHALCON_CALL_METHODW(NULL, getThis(), "offsetset", &key, &instance);
		} else {
			PHALCON_CALL_METHODW(NULL, getThis(), "offsetset", &key, value);
		}
	} ZEND_HASH_FOREACH_END();
}

/**
 * Allows to check whether an attribute is defined using the array-syntax
 *
 *<code>
 * var_dump(isset($config['database']));
 *</code>
 *
 * @param string $index
 * @return boolean
 */
PHP_METHOD(Phalcon_Config, offsetExists){

	zval *index;

	phalcon_fetch_params(0, 1, 0, &index);

	RETURN_BOOL(phalcon_isset_property_zval(getThis(), index));
}

/**
 * Gets an attribute from the configuration, if the attribute isn't defined returns null
 * If the value is exactly null or is not defined the default value will be used instead
 *
 *<code>
 * echo $config->get('controllersDir', '../app/controllers/');
 *</code>
 *
 * @param string $index
 * @param mixed $defaultValue
 * @return mixed
 */
PHP_METHOD(Phalcon_Config, get){

	zval *index, *default_value = NULL;

	phalcon_fetch_params(0, 1, 1, &index, &default_value);

	zval *tmp = phalcon_read_property_zval(getThis(), index, PH_NOISY);
	if (tmp) {
		RETURN_ZVAL(tmp, 1, 0);
	} else if (default_value) {
		RETURN_ZVAL(default_value, 1, 0);
	} else {
		RETURN_NULL();
	}
}

/**
 * Gets an attribute using the array-syntax
 *
 *<code>
 * print_r($config['database']);
 *</code>
 *
 * @param string $index
 * @return string
 */
PHP_METHOD(Phalcon_Config, offsetGet){

	zval *index;

	phalcon_fetch_params(0, 1, 0, &index);
	PHALCON_ENSURE_IS_STRING(index);
	phalcon_return_property_zval(return_value, getThis(), index);
}

/**
 * Sets an attribute using the array-syntax
 *
 *<code>
 * $config['database'] = array('type' => 'Sqlite');
 *</code>
 *
 * @param string $index
 * @param mixed $value
 */
PHP_METHOD(Phalcon_Config, offsetSet){

	zval *index, *value;

	phalcon_fetch_params(0, 2, 0, &index, &value);
	PHALCON_ENSURE_IS_STRING(index);
	phalcon_update_property_zval_zval(getThis(), index, value);
}

/**
 * Unsets an attribute using the array-syntax
 *
 *<code>
 * unset($config['database']);
 *</code>
 *
 * @param string $index
 */
PHP_METHOD(Phalcon_Config, offsetUnset){

	zval *index;

	phalcon_fetch_params(0, 1, 0, &index);
	PHALCON_ENSURE_IS_STRING(index);
	phalcon_update_property_zval_null(getThis(), index);
}

/**
 * Merges a configuration into the current one
 *
 * @brief void Phalcon\Config::merge(array|object $with)
 *
 *<code>
 *	$appConfig = new Phalcon\Config(array('database' => array('host' => 'localhost')));
 *	$globalConfig->merge($config2);
 *</code>
 *
 * @param Phalcon\Config $config
 * @return Phalcon\Config
 */
PHP_METHOD(Phalcon_Config, merge){

	zval *config, array_config, *value;
	zend_string *str_key;
	ulong idx;

	phalcon_fetch_params(0, 1, 0, &config);

	if (Z_TYPE_P(config) != IS_OBJECT && Z_TYPE_P(config) != IS_ARRAY) {
		PHALCON_THROW_EXCEPTION_STRW(phalcon_config_exception_ce, "Configuration must be an object or array");
		return;
	}

	if (Z_TYPE_P(config) == IS_OBJECT) {
		phalcon_get_object_vars(&array_config, config, 0);
	} else {
		PHALCON_CPY_WRT(&array_config, config);
	}

	if (Z_TYPE(array_config) == IS_ARRAY) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(array_config), idx, str_key, value) {
			zval tmp, *active_value;
			if (str_key) {
				ZVAL_STR(&tmp, str_key);
			} else {
				ZVAL_LONG(&tmp, idx);
			}

			if (phalcon_isset_property_zval(getThis(), &tmp)) {
				active_value = phalcon_read_property_zval(getThis(), &tmp, PH_NOISY);
				if ((Z_TYPE_P(value)  == IS_OBJECT || Z_TYPE_P(value) == IS_ARRAY) && Z_TYPE_P(active_value) == IS_OBJECT) {
					if (phalcon_method_exists_ex(active_value, SL("merge")) == SUCCESS) { /* Path AAA in the test */
						zval *params[] = { value };
						if (FAILURE == phalcon_call_method(NULL, active_value, "merge", 1, params)) {
							break;
						}
					} else {
						phalcon_update_property_zval_zval(getThis(), &tmp, value);
					}
				} else {
					phalcon_update_property_zval_zval(getThis(), &tmp, value);
				}
			} else {
				phalcon_update_property_zval_zval(getThis(), &tmp, value);
			}
		} ZEND_HASH_FOREACH_END();
	}

	RETURN_THISW();
}

/**
 * Converts recursively the object to an array
 *
 * @brief array Phalcon\Config::toArray(bool $recursive = true);
 *
 *<code>
 *	print_r($config->toArray());
 *</code>
 *
 * @return array
 */
PHP_METHOD(Phalcon_Config, toArray){

	zval *recursive = NULL, *value;
	zend_string *key;
	ulong idx;

	phalcon_fetch_params(0, 0, 1, &recursive);

	phalcon_get_object_vars(return_value, getThis(), 0);

	if (!recursive || zend_is_true(recursive)) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(return_value), idx, key, value) {
			zval tmp, array_value;
			if (key) {
				ZVAL_STR(&tmp, key);
			} else {
				ZVAL_LONG(&tmp, idx);
			}
			if (Z_TYPE_P(value) == IS_OBJECT && phalcon_method_exists_ex(value, SL("toarray")) == SUCCESS) {
				if (SUCCESS == phalcon_call_method(&array_value, value, "toarray", 0, NULL)) {
					phalcon_array_update_zval(return_value, &tmp, &array_value, PH_COPY);
				}

			}
		} ZEND_HASH_FOREACH_END();
	}
}

PHP_METHOD(Phalcon_Config, count)
{
	zval arr;

	phalcon_get_object_vars(&arr, getThis(), 0);
	RETURN_LONG(phalcon_fast_count_int(&arr));
}

PHP_METHOD(Phalcon_Config, __wakeup)
{
}

/**
 * Restores the state of a Phalcon\Config object
 *
 * @param array $data
 * @return Phalcon\Config
 */
PHP_METHOD(Phalcon_Config, __set_state){

	zval *data;

	phalcon_fetch_params(0, 1, 0, &data);

	object_init_ex(return_value, phalcon_config_ce);
	PHALCON_CALL_METHODW(NULL, return_value, "__construct", data);
}
