CREATE OR REPLACE FUNCTION notify_trigger() RETURNS trigger AS $$
DECLARE
    dataRecord RECORD;
BEGIN
    CASE TG_OP
    WHEN 'INSERT', 'UPDATE' THEN
        dataRecord := NEW;   
    WHEN 'DELETE' THEN
	dataRecord := OLD;
    END CASE;
    PERFORM pg_notify(CAST('watcher' AS text),
                      CONCAT('{"TableName":"',
                             TG_TABLE_NAME,
                             '", "Operation": "',
                             TG_OP,
                             '", "Data": ',
                             to_json(dataRecord),
                             '}')
                     );
    RETURN dataRecord;
END;
$$ LANGUAGE plpgsql;