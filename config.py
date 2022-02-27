config = [
        {
            "Manufacturer": "Dewenwils",
            "Name": "FivePack",
            "BasePattern": 0x08A20A0,
            "OffPattern": 0x00,
            "OnPattern": 0x10,
            "UnitCodes": {
                "1": 0x8, "2": 0x4, "3": 0x2, "4": 0xA, "5": 0x6
                },
            "NumberOfBitsInPattern": 25,
            "BitPeriodMicroseconds": 1076,
            },
        {
            "Manufacturer": "Dewenwils",
            "Name": "SingleUnit",
            "BasePattern": 0x0240048,
            "OffPattern": 0x00,
            "OnPattern": 0x10,
            "UnitCodes": {
                "0": 0x0,
                },
            "NumberOfBitsInPattern": 25,
            "BitPeriodMicroseconds": 1076,
            },
        {
            # The details here aren't quite right: it seems to switch sometimes,
            # but not consistently.  None of my remote controls work any more,
            # so I can't compare with the real thing to verify operation.
            "Manufacturer": "Energenie",
            "Name": "FourPack",
            "BasePattern": 0x1BD2D20,
            "OffPattern": 0x00,
            "OnPattern": 0x02,
            "UnitCodes": {
                "1": 0x1C, "2": 0x0C, "3": 0x14, "4": 0x04
                },
            "NumberOfBitsInPattern": 25,
            "BitPeriodMicroseconds": 2188,
            # Including this overrides the default 25%/75% on time
            "OnTimeOverrides": {
                "0": 570,
                "1": 1640,
                },
            },
        {
            # Everything in the Etekcity spec is likely to be wrong!
            # My remote controls have stopped working, so I can't get
            # the code or the frequency out properly.
            "Manufacturer": "Etekcity",
            "Name": "ThreePack",
            "BasePattern": 0x028C006,
            "OffPattern": 0x00,
            "OnPattern": 0x00,
            "UnitCodes": {
                "1": 0x0C60, "2": 0x0D80, "3": 0x1E00
                },
            "NumberOfBitsInPattern": 25,
            "BitPeriodMicroseconds": 1708,
            },
        ]
