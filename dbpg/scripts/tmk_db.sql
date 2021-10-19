CREATE TABLE "mandrels" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "tech_id" integer NOT NULL,
  "number" TEXT NOT NULL DEFAULT '----',
  "diameter" FLOAT NOT NULL,
  "last_detection" timestamp NOT NULL,
  "condition" BOOLEAN[20] NOT NULL
);

CREATE TABLE "mandrel_defects" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "mandrel_id" integer NOT NULL,
  "description_id" integer NOT NULL,
  "detected" timestamp NOT NULL,
  "position" FLOAT NOT NULL,
  "depth" FLOAT NOT NULL,
  "size" FLOAT NOT NULL,
  "image_url" TEXT NOT NULL
);

CREATE TABLE "billets" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "description_id" integer,
  "bar_id" integer,
  "tech_id" integer NOT NULL,
  "detected" timestamp NOT NULL,
  "diameter1" FLOAT NOT NULL,
  "diameter2" FLOAT NOT NULL,
  "diameter3" FLOAT NOT NULL,
  "length" FLOAT NOT NULL,
  "condition" BOOLEAN[20] NOT NULL,
  "image_url" TEXT NOT NULL
);

CREATE TABLE "billet_tech" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "name" TEXT NOT NULL,
  "diameter_min" FLOAT NOT NULL,
  "diameter_max" FLOAT NOT NULL,
  "length_min" FLOAT NOT NULL,
  "length_max" FLOAT NOT NULL
);

CREATE TABLE "bars" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "detected" timestamp,
  "order" TEXT NOT NULL,
  "steel_grade" TEXT NOT NULL,
  "mass" FLOAT NOT NULL
);

CREATE TABLE "pipe_defects" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "pipe_id" integer,
  "description_id" integer NOT NULL,
  "detected" timestamp NOT NULL,
  "detect_location" integer NOT NULL,
  "length" FLOAT NOT NULL,
  "depth" FLOAT NOT NULL,
  "position" FLOAT NOT NULL,
  "image_url" TEXT NOT NULL
);

CREATE TABLE "pipes" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "billet_id" integer,
  "mandrel_id" integer,
  "tech_id" integer NOT NULL,
  "detected" timestamp NOT NULL,
  "detect_locaton" integer NOT NULL,
  "condition" BOOLEAN[20] NOT NULL
);

CREATE TABLE "pipe_tech" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "name" TEXT NOT NULL,
  "depth_max" FLOAT NOT NULL,
  "size_max" FLOAT NOT NULL,
  "length_min" FLOAT NOT NULL,
  "length_max" FLOAT NOT NULL,
  "heater_pass" BOOLEAN NOT NULL
);

CREATE TABLE "mandrel_tech" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "name" TEXT NOT NULL,
  "depth_max" FLOAT NOT NULL,
  "size_max" FLOAT NOT NULL
);

CREATE TABLE "pipe_defect_description" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "name" TEXT NOT NULL,
  "description_url" TEXT NOT NULL
);

CREATE TABLE "mandrel_defect_description" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "name" TEXT NOT NULL,
  "description_url" TEXT NOT NULL
);

CREATE TABLE "billet_defect_description" (
  "id" SERIAL PRIMARY KEY NOT NULL,
  "name" TEXT NOT NULL,
  "description_url" TEXT NOT NULL
);

ALTER TABLE "mandrels" ADD FOREIGN KEY ("tech_id") REFERENCES "mandrel_tech" ("id");

ALTER TABLE "mandrel_defects" ADD FOREIGN KEY ("mandrel_id") REFERENCES "mandrels" ("id");

ALTER TABLE "mandrel_defects" ADD FOREIGN KEY ("description_id") REFERENCES "mandrel_defect_description" ("id");

ALTER TABLE "billets" ADD FOREIGN KEY ("description_id") REFERENCES "billet_defect_description" ("id");

ALTER TABLE "billets" ADD FOREIGN KEY ("bar_id") REFERENCES "bars" ("id");

ALTER TABLE "billets" ADD FOREIGN KEY ("tech_id") REFERENCES "billet_tech" ("id");

ALTER TABLE "pipe_defects" ADD FOREIGN KEY ("pipe_id") REFERENCES "pipes" ("id");

ALTER TABLE "pipe_defects" ADD FOREIGN KEY ("description_id") REFERENCES "pipe_defect_description" ("id");

ALTER TABLE "pipes" ADD FOREIGN KEY ("billet_id") REFERENCES "billets" ("id");

ALTER TABLE "pipes" ADD FOREIGN KEY ("mandrel_id") REFERENCES "mandrels" ("id");

ALTER TABLE "pipes" ADD FOREIGN KEY ("tech_id") REFERENCES "pipe_tech" ("id");
