"""
    Some tests that has no assertions but useful to watch for sent_to_winnum.py reacts for different cases.
"""


class TestDbMandrelInserter:
    """
    Mandrels tests collection
    """
    def test_insert_mandrel_with_2_defects(self, build_defect_mandrel_query_with_2_defects, pg_connect):
        pg_connect.execute(build_defect_mandrel_query_with_2_defects)

    def test_insert_bad_mandrel(self, build_bad_mandrel_query, pg_connect):
        pg_connect.execute(build_bad_mandrel_query)

    def test_insert_mandrel_with_no_defects(self, build_valid_mandrel_query, pg_connect):
        pg_connect.execute(build_valid_mandrel_query)


class TestDbPipeInserter:
    """
    Pipes tests collection
    """
    def test_insert_bad_pipe(self, build_bad_pipe_query, pg_connect):
        pg_connect.execute(build_bad_pipe_query)

    def test_insert_pipe_with_2_defects(self, build_defect_pipe_query_with_2_defects, pg_connect):
        pg_connect.execute(build_defect_pipe_query_with_2_defects)

    def test_insert_pipe_with_no_defects(self, build_valid_pipe_query, pg_connect):
        pg_connect.execute(build_valid_pipe_query)


class TestDbBilletInserter:
    """
    Billets tests collection
    """
    def test_insert_bad_billet(self, build_bad_billet_query, pg_connect):
        pg_connect.execute(build_bad_billet_query)

    def test_insert_billet_with_defect(self, build_defect_billet_query, pg_connect):
        pg_connect.execute(build_defect_billet_query)

    def test_insert_billet_with_no_defects(self, build_valid_billet_query, pg_connect):
        pg_connect.execute(build_valid_billet_query)