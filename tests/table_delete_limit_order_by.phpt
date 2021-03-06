--TEST--
mysqlx table delete/limit/orderBy
--SKIPIF--
--FILE--
<?php
	require("connect.inc");

	$nodeSession = create_test_db();

	fill_db_table_use_dup();

	$schema = $nodeSession->getSchema($db);
	$table = $schema->getTable('test_table');

	try {
		$table->delete()->where("name = :name")->orderby("id DESC")->limit(2)->bind(['name' => 'Cassidy'])->execute();
		test_step_failed();
	} catch(Exception $e) {
		test_step_ok();
	}

	try {
		$table->delete()->where("name = :name")->orderby("age DESC")->limit(-1)->bind(['name' => 'Tierney'])->execute();
		test_step_failed();
	} catch(Exception $e) {
		test_step_ok();
	}

	try {
		$table->delete()->where(['age = 17','name = \'Tierney\''])->execute();
		test_step_failed();
	} catch(Exception $e) {
		test_step_ok();
	}

        $res = $table->select('age', 'name')->execute()->fetchAll();
	expect_eq(count($res), 16);

	$table->delete()->where('name = :name')->orderby('age desc')->limit(2)->bind(['name' => 'Tierney'])->execute();
	$res = $table->select('name','age')->where('name like \'Tierney\'')->orderby('age desc')->execute()->fetchAll();

	expect_eq(count($res), 2);
	expect_eq($res[0]['name'],'Tierney');
	expect_eq($res[0]['age'],34);
	expect_eq($res[1]['name'],'Tierney');
	expect_eq($res[1]['age'],25);


	//TODO: Only zero value offset allowed for this operation
	$table->delete()->where('age = 17')->orderby('name desc')->limit(3)->offset(0)->execute();
	$res = $table->select('name','age')->where('age = 17')->execute()->fetchAll();
	expect_eq(count($res), 2);
	expect_eq($res[0]['name'],'ARomy');
	expect_eq($res[0]['age'],17);
	expect_eq($res[1]['name'],'BRomy');
	expect_eq($res[1]['age'],17);

	$nodeSession->executeSql("insert into $db.test_table values ('Zillon', 29)");
	$nodeSession->executeSql("insert into $db.test_table values ('Zillon', 21)");
	$nodeSession->executeSql("insert into $db.test_table values ('Zillon', 34)");

	$table->delete()->orderby(['name desc','age desc'])->limit(2)->execute();
	$res = $table->select('name','age')->where('name = \'Zillon\'')->execute()->fetchAll();
	expect_eq(count($res), 1);
	expect_eq($res[0]['name'],'Zillon');
	expect_eq($res[0]['age'],21);

	verify_expectations();
	print "done!\n";
?>
--CLEAN--
<?php
    require("connect.inc");
    clean_test_db();
?>
--EXPECTF--
done!%A
