<?php
	require_once(__DIR__."/../connect.inc");
	require_once(__DIR__."/mysqlx_row_locking.inc");

	function send_current_state($res1, $res2) {
		$result_msg = strval($res1['n']) . " " . strval($res2['n']);
		echo $result_msg, "\n";
	}

	notify_worker_started();

	$session = mysql_xdevapi\getSession($connection_uri);

	$schema = $session->getSchema($test_schema_name);
	$coll = $schema->getCollection($test_collection_name);

	$session->startTransaction();

	$res2 = find_lock_one($coll, '2', $Lock_shared);

	$res1 = find_lock_one($coll, '1', $Lock_shared);

	send_current_state($res1, $res2);

	recv_let_worker_commit();
	$session->commit();
	notify_worker_committed();

	$res1 = find_lock_one($coll, '1', $Lock_shared);
	$res2 = find_lock_one($coll, '2', $Lock_shared);

	send_current_state($res1, $res2);
?>
