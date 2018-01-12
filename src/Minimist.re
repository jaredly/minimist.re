
module StrMap = Map.Make(String);
module StrSet = Set.Make(String);

type result('a, 'b) = Ok('a) | Error('b);

type opts = {
  presence: StrSet.t,
  bools: StrMap.t(bool),
  floats: StrMap.t(float),
  ints: StrMap.t(int),
  strings: StrMap.t(string),
  multi: StrMap.t(list(string)),
  rest: list(string),
};

let empty = {
  presence: StrSet.empty,
  bools: StrMap.empty,
  floats: StrMap.empty,
  ints: StrMap.empty,
  strings: StrMap.empty,
  multi: StrMap.empty,
  rest: []
};

let maybeAssoc = (key, list) => try (Some(List.assoc(key, list))) { | Not_found => None };

let getValue = items => {
  switch items {
  | [] => None
  | [name, ...rest] => if (name.[0] == '-') {
    None
  } else {
    Some((name, rest))
  }
  }
};

let getBool = items => {
  switch (getValue(items)) {
  | Some(("true", rest)) => Some((true, rest))
  | Some(("false", rest)) => Some((false, rest))
  | _ => None
  }
};

let getFloat = items => switch (getValue(items)) {
| None => None
| Some((value, rest)) =>
  try (Some((float_of_string(value), rest))) {
  | _ => None
  }
};

let getInt = items => switch (getValue(items)) {
| None => None
| Some((value, rest)) =>
  try (Some((int_of_string(value), rest))) {
  | _ => None
  }
};

let parse = (~alias=[], ~presence=[], ~bools=[], ~floats=[], ~ints=[], ~strings=[], ~multi=[], args) => {
  let rec loop = (items, opts) => {
    switch items {
    | [] => Ok(opts)
    | [name, ...rest] => {
      let arg = if (name.[0] == '-') {
        Some(if (String.length(name) > 1 && name.[1] == '-') {
          String.sub(name, 2, String.length(name) - 2)
        } else {
          String.sub(name, 1, String.length(name) - 1)
        })
      } else {None};
      print_endline(name);
      switch arg {
      | None => loop(rest, {...opts, rest: [name, ...opts.rest]})
      | Some(name) =>
        let name = switch (maybeAssoc(name, alias)) {
        | None => name
        | Some(name) => name
        };
        let mem = List.mem(name);
        if (mem(presence)) {
          loop(rest, {
            ...opts,
            presence: StrSet.add(name, opts.presence)
          })
        } else if (mem(bools)) {
          switch (getBool(rest)) {
          | None => Error(`BadValue(name, "boolean"))
          | Some((value, rest)) =>
            loop(rest, {
              ...opts,
              bools: StrMap.add(name, value, opts.bools)
            })
          }
        } else if (mem(floats)) {
          switch (getFloat(rest)) {
          | None => Error(`BadValue(name, "float"))
          | Some((value, rest)) =>
            loop(rest, {
              ...opts,
              floats: StrMap.add(name, value, opts.floats)
            })
          }
        } else if (mem(ints)) {
          switch (getInt(rest)) {
          | None => Error(`BadValue(name, "integer"))
          | Some((value, rest)) =>
            loop(rest, {
              ...opts,
              ints: StrMap.add(name, value, opts.ints)
            })
          }
        } else if (mem(strings)) {
          switch (getValue(rest)) {
          | None => Error(`BadValue(name, "string"))
          | Some((value, rest)) =>
            loop(rest, {
              ...opts,
              strings: StrMap.add(name, value, opts.strings)
            })
          }
        } else if (mem(multi)) {
          switch (getValue(rest)) {
          | None => Error(`BadValue(name, "string"))
          | Some((value, rest)) =>
            loop(rest, {
              ...opts,
              multi: if (StrMap.mem(name, opts.multi)) {
                StrMap.add(name, [value, ...StrMap.find(name, opts.multi)], opts.multi)
              } else {
                StrMap.add(name, [value], opts.multi)
              }
            })
          }
        } else {
          Error(`Unknown(name))
        }
      }
    }
    }
  };
  loop(args, empty)
};

let get = (map, key) => if (StrMap.mem(key, map)) {
  Some(StrMap.find(key, map))
} else {
  None
};

let multi = (map, key) => if (StrMap.mem(key, map)) {
  StrMap.find(key, map)
} else {
  []
};

let report = (err) => switch err {
| `BadValue((arg, typ)) => ("Need a " ++ typ ++ " value for argument " ++ arg)
| `Unknown(name) => ("Unexpected argument: " ++ name)
};
