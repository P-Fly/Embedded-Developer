/**
 * Embedded Device Software
 * Copyright (C) 2020 Peter.Peng
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "drv_crc.h"
#include "drv_clock.h"
#include "log.h"

#include "stm32wbxx_hal.h"

#if defined(CONFIG_CRC_ENABLE)

/**
 * @brief   CRC handle definition.
 */
typedef struct {
	CRC_HandleTypeDef	crc;
	const object *		clock;
} stm32wbxx_crc_handle_t;

/**
 * @brief   Install one hardware configuration from a configuration space.
 *
 * @param   obj Pointer to the CRC object handle.
 * @param   config Pointer to the configuration space.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_crc_configure(const object *	obj,
				   const crc_config_t * config)
{
	stm32wbxx_crc_handle_t *handle =
		(stm32wbxx_crc_handle_t *)obj->object_data;

	if (!handle)
		return -EINVAL;

	if (!config)
		return -EINVAL;

	if (config->polynomial == CRC_CONFIG_POLYNOMIAL_DEFAULT) {
		handle->crc.Init.DefaultPolynomialUse =
			DEFAULT_POLYNOMIAL_ENABLE;
	} else {
		handle->crc.Init.DefaultPolynomialUse =
			DEFAULT_POLYNOMIAL_DISABLE;
		handle->crc.Init.GeneratingPolynomial = config->polynomial;
	}

	handle->crc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
	handle->crc.Init.InitValue = config->initial_value;
	handle->crc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
	handle->crc.Init.OutputDataInversionMode =
		CRC_OUTPUTDATA_INVERSION_DISABLE;

	/* Set Polynomial Length */
	switch (CRC_CONFIG_POLYNOMIAL_LENGTH_MASK(config->configs)) {
	case CRC_CONFIG_POLYNOMIAL_LENGTH_32B:
		handle->crc.Init.CRCLength = CRC_POLYLENGTH_32B;
		break;
	case CRC_CONFIG_POLYNOMIAL_LENGTH_16B:
		handle->crc.Init.CRCLength = CRC_POLYLENGTH_16B;
		break;
	case CRC_CONFIG_POLYNOMIAL_LENGTH_8B:
		handle->crc.Init.CRCLength = CRC_POLYLENGTH_8B;
		break;
	case CRC_CONFIG_POLYNOMIAL_LENGTH_7B:
		handle->crc.Init.CRCLength = CRC_POLYLENGTH_7B;
		break;
	default:
		return -EINVAL;
	}

	/* Set Input Format */
	switch (CRC_CONFIG_INPUT_MASK(config->configs)) {
	case CRC_CONFIG_INPUT_32B:
		handle->crc.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
		break;
	case CRC_CONFIG_INPUT_16B:
		handle->crc.InputDataFormat = CRC_INPUTDATA_FORMAT_HALFWORDS;
		break;
	case CRC_CONFIG_INPUT_8B:
		handle->crc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
		break;
	default:
		return -EINVAL;
	}

	if (HAL_CRC_Init(&handle->crc) != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief   Compute the CRC value of an data buffer.
 *
 * @param   obj Pointer to the CRC object handle.
 * @param   buf Pointer to data buffer to compute.
 * @param   len Data length to compute.
 * @param   crc CRC (returned value LSBs for CRC shorter than 32 bits).
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_crc_calculate(const object *obj,
				   const void *buf, int len, unsigned int *crc)
{
	stm32wbxx_crc_handle_t *handle =
		(stm32wbxx_crc_handle_t *)obj->object_data;

	if (!handle)
		return -EINVAL;

	if (!buf)
		return -EINVAL;

	if (len < 0)
		return -EINVAL;

	if (!crc)
		return -EINVAL;

	*crc = HAL_CRC_Calculate(&handle->crc, (uint32_t *)buf, len);

	return 0;
}

/**
 * @brief   Compute the CRC value of an data buffer,
 *          starting with the previously computed CRC as initialization value.
 *
 * @param   obj Pointer to the CRC object handle.
 * @param   buf Pointer to data buffer to compute.
 * @param   len Data length to compute.
 * @param   crc CRC (returned value LSBs for CRC shorter than 32 bits).
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_crc_accumulate(const object *obj,
				    const void *buf, int len, unsigned int *crc)
{
	stm32wbxx_crc_handle_t *handle =
		(stm32wbxx_crc_handle_t *)obj->object_data;

	if (!handle)
		return -EINVAL;

	if (!buf)
		return -EINVAL;

	if (len < 0)
		return -EINVAL;

	if (!crc)
		return -EINVAL;

	*crc = HAL_CRC_Accumulate(&handle->crc, (uint32_t *)buf, len);

	return 0;
}

static crc_intf_t crc_intf =
{
	.configure	= stm32wbxx_crc_configure,
	.calculate	= stm32wbxx_crc_calculate,
	.accumulate	= stm32wbxx_crc_accumulate,
};

static stm32wbxx_crc_handle_t crc_handle;

static crc_config_t crc_config =
{
	.initial_value	= CONFIG_CRC_HW_INITIAL_VALUE,
	.polynomial	= CONFIG_CRC_HW_POLYNOMIAL,
	.configs	= CONFIG_CRC_HW_CONFIGS,
};

/**
 * @brief   Probe the CRC driver.
 *
 * @param   obj Pointer to the CRC object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_crc_probe(const object *obj)
{
	stm32wbxx_crc_handle_t *handle =
		(stm32wbxx_crc_handle_t *)obj->object_data;
	int ret;

	(void)memset(handle, 0, sizeof(stm32wbxx_crc_handle_t));

	handle->crc.Instance = CRC;

	handle->clock = object_get_binding(CONFIG_CLOCK_NAME);
	if (!handle->clock) {
		pr_error("Drivce <%s> binding object <%s> failed.",
			 obj->name,
			 CONFIG_CLOCK_NAME);
		return -ENODEV;
	} else {
		pr_info("Drivce <%s> binding object <%s> succeed.",
			obj->name,
			CONFIG_CLOCK_NAME);
	}

	ret = clock_on(handle->clock, DRV_CLK_CRC, 0);
	if (ret)
		return ret;

	ret = crc_configure(obj, obj->object_config);
	if (ret)
		return ret;

	pr_info("Object <%s> probe succeed.", obj->name);

	return 0;
}

/**
 * @brief   Remove the CRC driver.
 *
 * @param   obj Pointer to the CRC object handle.
 *
 * @retval  Returns 0 on success, negative error code otherwise.
 */
static int stm32wbxx_crc_shutdown(const object *obj)
{
	stm32wbxx_crc_handle_t *handle =
		(stm32wbxx_crc_handle_t *)obj->object_data;
	int ret;

	if (HAL_CRC_DeInit(&handle->crc) != HAL_OK)
		return -EIO;

	ret = clock_off(handle->clock, DRV_CLK_CRC, 0);
	if (ret)
		return ret;

	pr_info("Object <%s> shutdown succeed.", obj->name);

	return 0;
}

module_driver(CONFIG_CRC_NAME,
	      CONFIG_CRC_LABEL,
	      stm32wbxx_crc_probe,
	      stm32wbxx_crc_shutdown,
	      &crc_intf, &crc_handle, &crc_config);

#endif
