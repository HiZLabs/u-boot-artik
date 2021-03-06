/*
 * (C) Copyright 2013 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <common.h>
#include <asm/arch/cpu.h>
#include "pmic.h"

void Delay(void)
{
	unsigned long i;
	for (i = 0; i < DELAY; i++)
		;
}

void IIC0_SCLH_SDAH(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLH_SDAL(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_SCLL_SDAH(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLL_SDAL(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_ELow(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLL_SDAL();
}

void IIC0_EHigh(void)
{
	IIC0_SCLL_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLL_SDAH();
}

void IIC0_EStart(void)
{
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLL_SDAL();
}

void IIC0_EEnd(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLH_SDAH();
}

void IIC0_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	Delay();
	IIC0_ESCL_Hi;
	Delay();
	ack = GPIO_DAT;
	IIC0_ESCL_Hi;
	Delay();
	IIC0_ESCL_Hi;
	Delay();

	/* Function <- Output (SDA) */
	IIC0_ESDA_OUTP;

	ack = (ack >> GPIO_DAT_SHIFT) & 0x1;

	IIC0_SCLL_SDAL();
}

void IIC0_EAck_read(void)
{
	/* Function <- Output */
	IIC0_ESDA_OUTP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	IIC0_ESCL_Hi;
	IIC0_ESCL_Hi;
	/* Function <- Input (SDA) */
	IIC0_ESDA_INP;

	IIC0_SCLL_SDAL();
}

void IIC0_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	DIS_GPIO_PUD;

	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;

	/* Function <- Output (SCL) */
	IIC0_ESCL_OUTP;
	/* Function <- Output (SDA) */
	IIC0_ESDA_OUTP;

	Delay();
}

void IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* write */
	IIC0_ELow();

	/* ACK */
	IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	IIC0_EEnd();
}

void IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* write */
	IIC0_ELow();

	/* ACK */
	IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* read */
	IIC0_EHigh();
	/* ACK */
	IIC0_EAck_write();

	/* read reg. data. */
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	Delay();

	for (i = 8; i > 0; i--) {
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		reg = GPIO_DAT;
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();

		reg = (reg >> GPIO_DAT_SHIFT) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IIC0_EAck_read();
	IIC0_ESDA_OUTP;

	IIC0_EEnd();

	*IicData = data;
}

void pmic_init(void)
{
	float vdd_arm, vdd_kfc;
	float vdd_int, vdd_g3d, vdd_mif;

	vdd_arm = CONFIG_PM_CALC_VOLT(CONFIG_PM_VDD_ARM);
	vdd_kfc = CONFIG_PM_CALC_VOLT(CONFIG_PM_VDD_KFC);
	vdd_mif = CONFIG_PM_CALC_VOLT(CONFIG_PM_VDD_MIF);
	vdd_int = CONFIG_PM_CALC_VOLT(CONFIG_PM_VDD_INT);
	vdd_g3d = CONFIG_PM_CALC_VOLT(CONFIG_PM_VDD_G3D);

	IIC0_ESetport();
	/* BUCK2 VDD_ARM */
	IIC0_EWrite(S2MPS11_ADDR, BUCK2_CTRL2, vdd_arm);
	/* BUCK6 VDD_KFC */
	IIC0_EWrite(S2MPS11_ADDR, BUCK6_CTRL2, vdd_kfc);
	/* BUCK1 VDD_MIF */
	IIC0_EWrite(S2MPS11_ADDR, BUCK1_CTRL2, vdd_mif);
	/* BUCK3 VDD_INT */
	IIC0_EWrite(S2MPS11_ADDR, BUCK3_CTRL2, vdd_int);
	/* BUCK4 VDD_G3D */
	IIC0_EWrite(S2MPS11_ADDR, BUCK4_CTRL2, vdd_g3d);

#ifdef CONFIG_CPU_EXYNOS5422_EVT0
#ifdef CONFIG_MACH_UNIVERSAL5422
	/* enable LDO21 for Companion PMIC I2C control */
	IIC0_EWrite(S2MPS11_ADDR, 0x51, 0xD4);
#endif
#endif
#if defined(CONFIG_MACH_XYREF5422) || defined(CONFIG_MACH_ARTIK10)
	IIC0_EWrite(S2MPS11_ADDR, 0x4C, 0xD4);
	IIC0_EWrite(S2MPS11_ADDR, 0x54, 0xE8);
	IIC0_EWrite(S2MPS11_ADDR, 0x4E, 0xE8);
#endif
}

void pmic_turnon_vdd_lcd(void)
{
#ifdef CONFIG_CPU_EXYNOS5422_EVT0
   	//IIC0_EWrite(S2MPS11_ADDR, 0x4e, 0xD4); //LDO18 1.8V
  	IIC0_EWrite(S2MPS11_ADDR, 0x59, 0xD4); //LDO29 1.8V
#endif
}

void pmic_turnoff_vdd_lcd(void)
{
#ifdef CONFIG_CPU_EXYNOS5422_EVT0
#endif
}

int get_boot_cause(void)
{
#ifdef CONFIG_CPU_EXYNOS5422_EVT0
	unsigned int temp = SW_RST_STAT_REG;
	printf("Boot Cause =0x%x\n",temp);

	if(temp & (1<<PINRESET_RESET))
		return PINRESET_RESET;
	else if(temp & (1<<WARM_RESET))
		return WARM_RESET;
	else if(temp & (1<<SOFTWARE_RESET))
		return SOFTWARE_RESET;
	else
		return -1;
	return -1;
#endif
}

