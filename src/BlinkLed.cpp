/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "BlinkLed.h"

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

#define BLINK_GPIOx(_N)       ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BLINK_PIN_MASK(_N)    (1 << (_N))
#define BLINK_RCC_MASKx(_N)   (RCC_AHB1ENR_GPIOAEN << (_N))

// ----------------------------------------------------------------------------

BlinkLed::BlinkLed (unsigned int port, unsigned int bit, bool activeLow)
{
  fPortNumber = port;
  fBitNumber = bit;
  fIsActiveLow = activeLow;
  fBitMask = BLINK_PIN_MASK(fBitNumber);
}

void
BlinkLed::powerUp ()
{
  // Enable GPIO Peripheral clock
  RCC->AHB1ENR |= BLINK_RCC_MASKx(fPortNumber);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.Pin = fBitMask;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init (BLINK_GPIOx(fPortNumber), &GPIO_InitStructure);

  // Start with led turned off
  turnOff ();
}

void
BlinkLed::turnOn ()
{
  if (fIsActiveLow)
    {
      BLINK_GPIOx(fPortNumber)->BSRR = BLINK_PIN_MASK(fBitNumber + 16);
    }
  else
    {
      BLINK_GPIOx(fPortNumber)->BSRR = fBitMask;
    }
}

void
BlinkLed::turnOff ()
{
  if (fIsActiveLow)
    {
      BLINK_GPIOx(fPortNumber)->BSRR = fBitMask;
    }
  else
    {
      BLINK_GPIOx(fPortNumber)->BSRR = BLINK_PIN_MASK(fBitNumber + 16);
    }
}

void
BlinkLed::toggle ()
{
  if (BLINK_GPIOx(fPortNumber)->IDR & fBitMask)
    {
      BLINK_GPIOx(fPortNumber)->ODR &= ~fBitMask;
    }
  else
    {
      BLINK_GPIOx(fPortNumber)->ODR |= fBitMask;
    }
}

bool
BlinkLed::isOn ()
{
  if (fIsActiveLow)
    {
      return (BLINK_GPIOx(fPortNumber)->IDR & fBitMask) == 0;
    }
  else
    {
      return (BLINK_GPIOx(fPortNumber)->IDR & fBitMask) != 0;
    }
}

// ----------------------------------------------------------------------------
