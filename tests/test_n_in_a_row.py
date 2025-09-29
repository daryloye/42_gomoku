from ui.stones import Stones

def test_four_in_a_row():
    cfg = Config()
    rules = Rules(cfg)
    colour = cfg.colour.black

    # _xxxx_
    stones = Stones(cfg)
    stones.place((10, 10), colour)
    stones.place((11, 10), colour)
    stones.place((12, 10), colour)
    stones.place((13, 10), colour)
    available_moves = rules.check_four_in_a_row(stones, colour)
    assert (9, 10) in available_moves
    assert (14, 10) in available_moves
    assert len(available_moves) == 2

    # _x_xxx_
    stones = Stones(cfg)
    stones.place((10, 10), colour)
    stones.place((12, 10), colour)
    stones.place((13, 10), colour)
    stones.place((14, 10), colour)
    available_moves = rules.check_four_in_a_row(stones, colour)
    assert (11, 10) in available_moves
    assert len(available_moves) == 1

    # _xx_xx_
    stones = Stones(cfg)
    stones.place((10, 10), colour)
    stones.place((11, 10), colour)
    stones.place((13, 10), colour)
    stones.place((14, 10), colour)
    available_moves = rules.check_four_in_a_row(stones, colour)
    assert (12, 10) in available_moves
    assert len(available_moves) == 1